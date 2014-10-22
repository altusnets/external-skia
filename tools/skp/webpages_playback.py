#!/usr/bin/env python
# Copyright (c) 2012 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.

"""Archives or replays webpages and creates SKPs in a Google Storage location.

To archive webpages and store SKP files (archives should be rarely updated):

cd ../buildbot/slave/skia_slave_scripts
python webpages_playback.py --dest_gsbase=gs://rmistry --record \
--page_sets=all --skia_tools=/home/default/trunk/out/Debug/ \
--browser_executable=/tmp/chromium/out/Release/chrome


To replay archived webpages and re-generate SKP files (should be run whenever
SkPicture.PICTURE_VERSION changes):

cd ../buildbot/slave/skia_slave_scripts
python webpages_playback.py --dest_gsbase=gs://rmistry \
--page_sets=all --skia_tools=/home/default/trunk/out/Debug/ \
--browser_executable=/tmp/chromium/out/Release/chrome


Specify the --page_sets flag (default value is 'all') to pick a list of which
webpages should be archived and/or replayed. Eg:

--page_sets=page_sets/skia_yahooanswers_desktop.json,\
page_sets/skia_wikipedia_galaxynexus.json

The --browser_executable flag should point to the browser binary you want to use
to capture archives and/or capture SKP files. Majority of the time it should be
a newly built chrome binary.

The --upload_to_gs flag controls whether generated artifacts will be uploaded
to Google Storage (default value is False if not specified).

The --non-interactive flag controls whether the script will prompt the user
(default value is False if not specified).

The --skia_tools flag if specified will allow this script to run
debugger, render_pictures, and render_pdfs on the captured
SKP(s). The tools are run after all SKPs are succesfully captured to make sure
they can be added to the buildbots with no breakages.
To preview the captured SKP before proceeding to the next page_set specify both
--skia_tools and --view_debugger_output.
"""

import glob
import optparse
import os
import posixpath
import shutil
import subprocess
import sys
import tempfile
import time
import traceback

sys.path.insert(0, os.getcwd())

from common.py.utils import gs_utils
from common.py.utils import shell_utils

ROOT_PLAYBACK_DIR_NAME = 'playback'
SKPICTURES_DIR_NAME = 'skps'


# Local archive and SKP directories.
LOCAL_PLAYBACK_ROOT_DIR = os.path.join(
    tempfile.gettempdir(), ROOT_PLAYBACK_DIR_NAME)
LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR = os.path.join(
    os.path.abspath(os.path.dirname(__file__)), 'page_sets', 'data')
TMP_SKP_DIR = tempfile.mkdtemp()

# Location of the credentials.json file and the string that represents missing
# passwords.
CREDENTIALS_FILE_PATH = os.path.join(
    os.path.abspath(os.path.dirname(__file__)), 'page_sets', 'data',
    'credentials.json'
)

# Stdout that signifies that a recording has failed.
RECORD_FAILURE_MSG = 'The recording has not been updated for these pages.'

# Name of the SKP benchmark
SKP_BENCHMARK = 'skpicture_printer'

# The max base name length of Skp files.
MAX_SKP_BASE_NAME_LEN = 31

# Dictionary of device to platform prefixes for SKP files.
DEVICE_TO_PLATFORM_PREFIX = {
    'desktop': 'desk',
    'galaxynexus': 'mobi',
    'nexus10': 'tabl'
}

# How many times the record_wpr binary should be retried.
RETRY_RECORD_WPR_COUNT = 5
# How many times the run_benchmark binary should be retried.
RETRY_RUN_MEASUREMENT_COUNT = 5

# Location of the credentials.json file in Google Storage.
CREDENTIALS_GS_PATH = '/playback/credentials/credentials.json'

X11_DISPLAY = os.getenv('DISPLAY', ':0')

GS_PREDEFINED_ACL = gs_utils.GSUtils.PredefinedACL.PRIVATE
GS_FINE_GRAINED_ACL_LIST = [
  (gs_utils.GSUtils.IdType.GROUP_BY_DOMAIN, 'google.com',
   gs_utils.GSUtils.Permission.READ),
]


class SkPicturePlayback(object):
  """Class that archives or replays webpages and creates SKPs."""

  def __init__(self, parse_options):
    """Constructs a SkPicturePlayback BuildStep instance."""
    assert parse_options.browser_executable, 'Must specify --browser_executable'
    self._browser_executable = parse_options.browser_executable

    self._all_page_sets_specified = parse_options.page_sets == 'all'
    self._page_sets = self._ParsePageSets(parse_options.page_sets)

    self._dest_gsbase = parse_options.dest_gsbase
    self._record = parse_options.record
    self._skia_tools = parse_options.skia_tools
    self._non_interactive = parse_options.non_interactive
    self._upload_to_gs = parse_options.upload_to_gs
    self._alternate_upload_dir = parse_options.alternate_upload_dir
    self._skip_all_gs_access = parse_options.skip_all_gs_access
    self._telemetry_binaries_dir = os.path.join(parse_options.chrome_src_path,
                                                'tools', 'perf')

    self._local_skp_dir = os.path.join(
        parse_options.output_dir, ROOT_PLAYBACK_DIR_NAME, SKPICTURES_DIR_NAME)
    self._local_record_webpages_archive_dir = os.path.join(
        parse_options.output_dir, ROOT_PLAYBACK_DIR_NAME, 'webpages_archive')

    # List of SKP files generated by this script.
    self._skp_files = []

  def _ParsePageSets(self, page_sets):
    if not page_sets:
      raise ValueError('Must specify at least one page_set!')
    elif self._all_page_sets_specified:
      # Get everything from the page_sets directory.
      page_sets_dir = os.path.join(os.path.abspath(os.path.dirname(__file__)),
                                   'page_sets')
      ps = [os.path.join(page_sets_dir, page_set)
            for page_set in os.listdir(page_sets_dir)
            if not os.path.isdir(os.path.join(page_sets_dir, page_set)) and
               page_set.endswith('.py')]
    elif '*' in page_sets:
      # Explode and return the glob.
      ps = glob.glob(page_sets)
    else:
      ps = page_sets.split(',')
    ps.sort()
    return ps

  def Run(self):
    """Run the SkPicturePlayback BuildStep."""

    # Download the credentials file if it was not previously downloaded.
    if self._skip_all_gs_access:
      print """\n\nPlease create a %s file that contains:
      {
        "google": {
          "username": "google_testing_account_username",
          "password": "google_testing_account_password"
        },
        "facebook": {
          "username": "facebook_testing_account_username",
          "password": "facebook_testing_account_password"
        }
      }\n\n""" % CREDENTIALS_FILE_PATH
      raw_input("Please press a key when you are ready to proceed...")
    elif not os.path.isfile(CREDENTIALS_FILE_PATH):
      # Download the credentials.json file from Google Storage.
      gs_bucket = self._dest_gsbase.lstrip(gs_utils.GS_PREFIX)
      gs_utils.GSUtils().download_file(gs_bucket, CREDENTIALS_GS_PATH,
                                       CREDENTIALS_FILE_PATH)

    # Delete any left over data files in the data directory.
    for archive_file in glob.glob(
        os.path.join(LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR, 'skia_*')):
      os.remove(archive_file)

    # Delete the local root directory if it already exists.
    if os.path.exists(LOCAL_PLAYBACK_ROOT_DIR):
      shutil.rmtree(LOCAL_PLAYBACK_ROOT_DIR)

    # Create the required local storage directories.
    self._CreateLocalStorageDirs()

    # Start the timer.
    start_time = time.time()

    # Loop through all page_sets.
    for page_set in self._page_sets:

      page_set_basename = os.path.basename(page_set).split('.')[0] + '.json'
      wpr_data_file = page_set.split(os.path.sep)[-1].split('.')[0] + '_000.wpr'

      if self._record:
        # Create an archive of the specified webpages if '--record=True' is
        # specified.
        record_wpr_cmd = (
          'DISPLAY=%s' % X11_DISPLAY,
          os.path.join(self._telemetry_binaries_dir, 'record_wpr'),
          '--extra-browser-args=--disable-setuid-sandbox',
          '--browser=exact',
          '--browser-executable=%s' % self._browser_executable,
          page_set
        )
        for _ in range(RETRY_RECORD_WPR_COUNT):
          output = shell_utils.run(' '.join(record_wpr_cmd), shell=True)
          if RECORD_FAILURE_MSG in output:
            print output
          else:
            # Break out of the retry loop since there were no errors.
            break
        else:
          # If we get here then record_wpr did not succeed and thus did not
          # break out of the loop.
          raise Exception('record_wpr failed for page_set: %s' % page_set)

      else:
        if not self._skip_all_gs_access:
          # Get the webpages archive so that it can be replayed.
          self._DownloadWebpagesArchive(wpr_data_file, page_set_basename)

      page_set_name = os.path.basename(page_set).split('.')[0]
      page_set_dir = os.path.dirname(page_set)
      run_benchmark_cmd = (
          'PYTHONPATH=%s:$PYTHONPATH' % page_set_dir,
          'DISPLAY=%s' % X11_DISPLAY,
          'timeout', '300',
          os.path.join(self._telemetry_binaries_dir, 'run_benchmark'),
          '--extra-browser-args=--disable-setuid-sandbox',
          '--browser=exact',
          '--browser-executable=%s' % self._browser_executable,
          SKP_BENCHMARK,
          '--page-set-name=%s' % page_set_name,
          '--page-set-base-dir=%s' % page_set_dir,
          '--skp-outdir=%s' % TMP_SKP_DIR,
          '--also-run-disabled-tests'
      )

      for _ in range(RETRY_RUN_MEASUREMENT_COUNT):
        try:
          print '\n\n=======Capturing SKP of %s=======\n\n' % page_set
          shell_utils.run(' '.join(run_benchmark_cmd), shell=True)
        except shell_utils.CommandFailedException:
          # skpicture_printer sometimes fails with AssertionError but the
          # captured SKP is still valid. This is a known issue.
          pass

        if self._record:
          # Move over the created archive into the local webpages archive
          # directory.
          shutil.move(
              os.path.join(LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR, wpr_data_file),
              self._local_record_webpages_archive_dir)
          shutil.move(
              os.path.join(LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR,
                           page_set_basename),
              self._local_record_webpages_archive_dir)

        # Rename generated SKP files into more descriptive names.
        try:
          self._RenameSkpFiles(page_set)
          # Break out of the retry loop since there were no errors.
          break
        except Exception:
          # There was a failure continue with the loop.
          traceback.print_exc()
          print '\n\n=======Retrying %s=======\n\n' % page_set
          time.sleep(10)
      else:
        # If we get here then run_benchmark did not succeed and thus did not
        # break out of the loop.
        raise Exception('run_benchmark failed for page_set: %s' % page_set)

    print '\n\n=======Capturing SKP files took %s seconds=======\n\n' % (
        time.time() - start_time)

    if self._skia_tools:
      render_pictures_cmd = [
          os.path.join(self._skia_tools, 'render_pictures'),
          '-r', self._local_skp_dir
      ]
      render_pdfs_cmd = [
          os.path.join(self._skia_tools, 'render_pdfs'),
          self._local_skp_dir
      ]

      for tools_cmd in (render_pictures_cmd, render_pdfs_cmd):
        print '\n\n=======Running %s=======' % ' '.join(tools_cmd)
        proc = subprocess.Popen(tools_cmd)
        (code, output) = shell_utils.log_process_after_completion(proc,
                                                                  echo=False)
        if code != 0:
          raise Exception('%s failed!' % ' '.join(tools_cmd))

      if not self._non_interactive:
        print '\n\n=======Running debugger======='
        os.system('%s %s' % (os.path.join(self._skia_tools, 'debugger'),
                             os.path.join(self._local_skp_dir, '*')))

    print '\n\n'

    if not self._skip_all_gs_access and self._upload_to_gs:
      print '\n\n=======Uploading to Google Storage=======\n\n'
      # Copy the directory structure in the root directory into Google Storage.
      dest_dir_name = ROOT_PLAYBACK_DIR_NAME
      if self._alternate_upload_dir:
        dest_dir_name = self._alternate_upload_dir

      gs_bucket = self._dest_gsbase.lstrip(gs_utils.GS_PREFIX)
      gs_utils.GSUtils().upload_dir_contents(
          LOCAL_PLAYBACK_ROOT_DIR, gs_bucket, dest_dir_name,
          upload_if=gs_utils.GSUtils.UploadIf.IF_MODIFIED,
          predefined_acl=GS_PREDEFINED_ACL,
          fine_grained_acl_list=GS_FINE_GRAINED_ACL_LIST)

      print '\n\n=======New SKPs have been uploaded to %s =======\n\n' % (
          posixpath.join(self._dest_gsbase, dest_dir_name, SKPICTURES_DIR_NAME))
    else:
      print '\n\n=======Not Uploading to Google Storage=======\n\n'
      print 'Generated resources are available in %s\n\n' % (
          LOCAL_PLAYBACK_ROOT_DIR)

    return 0

  def _RenameSkpFiles(self, page_set):
    """Rename generated SKP files into more descriptive names.

    Look into the subdirectory of TMP_SKP_DIR and find the most interesting
    .skp in there to be this page_set's representative .skp.
    """
    # Here's where we're assuming there's one page per pageset.
    # If there were more than one, we'd overwrite filename below.

    # /path/to/skia_yahooanswers_desktop.json -> skia_yahooanswers_desktop.json
    _, ps_filename = os.path.split(page_set)
    # skia_yahooanswers_desktop.json -> skia_yahooanswers_desktop
    ps_basename, _ = os.path.splitext(ps_filename)
    # skia_yahooanswers_desktop -> skia, yahooanswers, desktop
    _, page_name, device = ps_basename.split('_')

    basename = '%s_%s' % (DEVICE_TO_PLATFORM_PREFIX[device], page_name)
    filename = basename[:MAX_SKP_BASE_NAME_LEN] + '.skp'

    subdirs = glob.glob(os.path.join(TMP_SKP_DIR, '*'))
    assert len(subdirs) == 1
    for site in subdirs:
      # We choose the largest .skp as the most likely to be interesting.
      largest_skp = max(glob.glob(os.path.join(site, '*.skp')),
                        key=lambda path: os.stat(path).st_size)
      dest = os.path.join(self._local_skp_dir, filename)
      print 'Moving', largest_skp, 'to', dest
      shutil.move(largest_skp, dest)
      self._skp_files.append(filename)
      shutil.rmtree(site)

  def _CreateLocalStorageDirs(self):
    """Creates required local storage directories for this script."""
    for d in (self._local_record_webpages_archive_dir,
              self._local_skp_dir):
      if os.path.exists(d):
        shutil.rmtree(d)
      os.makedirs(d)

  def _DownloadWebpagesArchive(self, wpr_data_file, page_set_basename):
    """Downloads the webpages archive and its required page set from GS."""
    wpr_source = posixpath.join(ROOT_PLAYBACK_DIR_NAME, 'webpages_archive',
                                wpr_data_file)
    page_set_source = posixpath.join(ROOT_PLAYBACK_DIR_NAME,
                                     'webpages_archive',
                                     page_set_basename)
    gs = gs_utils.GSUtils()
    gs_bucket = self._dest_gsbase.lstrip(gs_utils.GS_PREFIX)
    if (gs.does_storage_object_exist(gs_bucket, wpr_source) and
        gs.does_storage_object_exist(gs_bucket, page_set_source)):
      gs.download_file(gs_bucket, wpr_source,
                       os.path.join(LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR,
                                    wpr_data_file))
      gs.download_file(gs_bucket, page_set_source,
                       os.path.join(LOCAL_REPLAY_WEBPAGES_ARCHIVE_DIR,
                                    page_set_basename))
    else:
      raise Exception('%s and %s do not exist in Google Storage!' % (
          wpr_source, page_set_source))


if '__main__' == __name__:
  option_parser = optparse.OptionParser()
  option_parser.add_option(
      '', '--page_sets',
      help='Specifies the page sets to use to archive. Supports globs.',
      default='all')
  option_parser.add_option(
      '', '--skip_all_gs_access', action='store_true',
      help='All Google Storage interactions will be skipped if this flag is '
           'specified. This is useful for cases where the user does not have '
           'the required .boto file but would like to generate webpage '
           'archives and SKPs from the Skia page sets.',
      default=False)
  option_parser.add_option(
      '', '--record', action='store_true',
      help='Specifies whether a new website archive should be created.',
      default=False)
  option_parser.add_option(
      '', '--dest_gsbase',
      help='gs:// bucket_name, the bucket to upload the file to.',
      default='gs://chromium-skia-gm')
  option_parser.add_option(
      '', '--skia_tools',
      help=('Path to compiled Skia executable tools. '
            'render_pictures/render_pdfs is run on the set '
            'after all SKPs are captured. If the script is run without '
            '--non-interactive then the debugger is also run at the end. Debug '
            'builds are recommended because they seem to catch more failures '
            'than Release builds.'),
      default=None)
  option_parser.add_option(
      '', '--upload_to_gs', action='store_true',
      help='Does not upload to Google Storage if this is False.',
      default=False)
  option_parser.add_option(
      '', '--alternate_upload_dir',
      help='Uploads to a different directory in Google Storage if this flag is '
           'specified',
      default=None)
  option_parser.add_option(
      '', '--output_dir',
      help='Directory where SKPs and webpage archives will be outputted to.',
      default=tempfile.gettempdir())
  option_parser.add_option(
      '', '--browser_executable',
      help='The exact browser executable to run.',
      default=None)
  option_parser.add_option(
      '', '--chrome_src_path',
      help='Path to the chromium src directory.',
      default=None)
  option_parser.add_option(
      '', '--non-interactive', action='store_true',
      help='Runs the script without any prompts. If this flag is specified and '
           '--skia_tools is specified then the debugger is not run.',
      default=False)
  options, unused_args = option_parser.parse_args()

  playback = SkPicturePlayback(options)
  sys.exit(playback.Run())
