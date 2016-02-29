/*
 * Copyright 2016 Google Inc.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include "GrAuditTrail.h"
#include "batches/GrBatch.h"

void GrAuditTrail::batchingResultCombined(GrBatch* combiner) {
    int* indexPtr = fIDLookup.find(combiner);
    SkASSERT(indexPtr);
    int index = *indexPtr;
    SkASSERT(index < fBatches.count());
    Batch& batch = *fBatches[index];

    // if this is our first child, we also push back a copy of the original batch and its
    // bounds
    if (batch.fChildren.empty()) {
        Batch* firstBatch = new Batch;
        firstBatch->fName = batch.fName;
        firstBatch->fBounds = batch.fBounds;
        fEvents.emplace_back(firstBatch);
        batch.fChildren.push_back(firstBatch);
    } 
    batch.fChildren.push_back(fCurrentBatch);
    batch.fBounds = combiner->bounds();
}

void GrAuditTrail::batchingResultNew(GrBatch* batch) {
    fIDLookup.set(batch, fBatches.count());
    fBatches.push_back(fCurrentBatch);
}

template <typename T>
void GrAuditTrail::JsonifyTArray(SkString* json, const char* name, const T& array,
                                 bool addComma) {
    if (array.count()) {
        if (addComma) {
            json->appendf(",");
        }
        json->appendf("\"%s\": [", name);
        for (int i = 0; i < array.count(); i++) {
            json->append(array[i]->toJson());
            if (i < array.count() - 1) {
                json->append(",");
            }
        }
        json->append("]");
    }
}

// This will pretty print a very small subset of json
// The parsing rules are straightforward, aside from the fact that we do not want an extra newline
// before ',' and after '}', so we have a comma exception rule.
class PrettyPrintJson {
public:
    SkString prettify(const SkString& json) {
        fPrettyJson.reset();
        fTabCount = 0;
        fFreshLine = false;
        fCommaException = false;
        for (size_t i = 0; i < json.size(); i++) {
            if ('[' == json[i] || '{' == json[i]) {
                this->newline();
                this->appendChar(json[i]);
                fTabCount++;
                this->newline();
            } else if (']' == json[i] || '}' == json[i]) {
                fTabCount--;
                this->newline();
                this->appendChar(json[i]);
                fCommaException = true;
            } else if (',' == json[i]) {
                this->appendChar(json[i]);
                this->newline();
            } else {
                this->appendChar(json[i]);
            }
        }
        return fPrettyJson;
    }
private:
    void appendChar(char appendee) {
        if (fCommaException && ',' != appendee) {
            this->newline();
        }
        this->tab();
        fPrettyJson += appendee;
        fFreshLine = false;
        fCommaException = false;
    }

    void tab() {
        if (fFreshLine) {
            for (int i = 0; i < fTabCount; i++) {
                fPrettyJson += '\t';
            }
        }
    }

    void newline() {
        if (!fFreshLine) {
            fFreshLine = true;
            fPrettyJson += '\n';
        }
    }

    SkString fPrettyJson;
    int fTabCount;
    bool fFreshLine;
    bool fCommaException;
};

static SkString pretty_print_json(SkString json) {
    class PrettyPrintJson prettyPrintJson;
    return prettyPrintJson.prettify(json);
}

SkString GrAuditTrail::toJson(bool batchList, bool prettyPrint) const {
    SkString json;
    json.append("{");
    if (!batchList) {
        JsonifyTArray(&json, "Stacks", fFrames, false);
    } else {
        JsonifyTArray(&json, "Batches", fBatches, false);
    }
    json.append("}");

    if (prettyPrint) {
        return pretty_print_json(json);
    } else {
        return json;
    }
}

SkString GrAuditTrail::Frame::toJson() const {
    SkString json;
    json.append("{");
    json.appendf("\"Name\": \"%s\"", fName);
    JsonifyTArray(&json, "Frames", fChildren, true);
    json.append("}");
    return json;
}

SkString GrAuditTrail::Batch::toJson() const {
    SkString json;
    json.append("{");
    json.appendf("\"Name\": \"%s\",", fName);
    json.append("\"Bounds\": {");
    json.appendf("\"Left\": %f,", fBounds.fLeft);
    json.appendf("\"Right\": %f,", fBounds.fRight);
    json.appendf("\"Top\": %f,", fBounds.fTop);
    json.appendf("\"Bottom\": %f", fBounds.fBottom);
    JsonifyTArray(&json, "Children", fChildren, true);
    json.append("}");
    json.append("}");
    return json;
}
