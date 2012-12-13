/**
 * Copyright (c) 2012 Andrey "asfdfdfd" Panchenko
 * email:asfdfdfd@gmail.com
 *
 * Report bugs at https://github.com/asfdfdfd/fontbuilder
 *
 * This software is distributed under the MIT License.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include "jsexporter.h"
#include "../fontconfig.h"
#include "../layoutdata.h"

#include <QString>
JsExporter::JsExporter(QObject *parent) :
    AbstractExporter(parent)
{
    setExtension("js");
}
static QString charCode(uint code) {
    if (code=='\"') return QString().append('\'').append(code).append('\'');
    if (code=='\\') return QString("\"\\\\\"");
    return QString().append('\"').append(code).append('\"');
}

bool JsExporter::Export(QByteArray& out) {
    QString res = "var font = {\n";

    res+=QString("\t\"file\":\"")+texFilename()+QString("\",\n");
    res+=QString("\t\"height\":")+QString().number(metrics().height)+QString(",\n");
    res+=QString("\t\"description\":{\n");
    res+=QString("\t\t\"family\":\"")+fontConfig()->family()+QString("\",\n");
    res+=QString("\t\t\"style\":\"")+fontConfig()->style()+QString("\",\n");
    res+=QString("\t\t\"size\":")+QString().number(fontConfig()->size())+QString("\n");
    res+=QString("\t},\n");

    res+=QString("\t\"metrics\":{\n");
    res+=QString("\t\t\"ascender\":")+QString().number(metrics().ascender)+QString(",\n");
    res+=QString("\t\t\"descender\":")+QString().number(metrics().descender)+QString(",\n");
    res+=QString("\t\t\"height\":")+QString().number(metrics().height)+QString("\n");
    res+=QString("\t},\n");

    res+=QString("\t\"texture\":{\n");
    res+=QString("\t\t\"file\":\"")+texFilename()+QString("\",\n");
    res+=QString("\t\t\"width\":")+QString().number(texWidth())+QString(",\n");
    res+=QString("\t\t\"height\":")+QString().number(texHeight())+QString("\n");
    res+=QString("\t},\n");

    res+=QString("\t\"chars\":{\n");
    foreach (const Symbol& c , symbols()) {
        QString charDef=charCode(c.id);
        charDef+=QString(":{");
        charDef+=QString("\"width\":")+QString().number(c.advance)+QString(",");
        charDef+=QString("\"x\":")+QString().number(c.placeX)+QString(",");
        charDef+=QString("\"y\":")+QString().number(c.placeY)+QString(",");
        charDef+=QString("\"w\":")+QString().number(c.placeW)+QString(",");
        charDef+=QString("\"h\":")+QString().number(c.placeH)+QString(",");

        charDef+=QString("\"ox\":")+QString().number(c.offsetX)+QString(",");
        charDef+=QString("\"oy\":")+QString().number(c.offsetY)+QString("}");

        res+=QString("\t\t")+charDef+QString(",\n");
    }
    res+=QString("\t},\n");
    QString kernings;
    foreach (const Symbol& c , symbols()) {
        QString charDef="{\"from\":";
        charDef+=charCode(c.id);
        charDef+=QString(",\"to\":");
        typedef QMap<ushort,int>::ConstIterator Kerning;
        for ( Kerning k = c.kerning.begin();k!=c.kerning.end();k++) {
            QString def = charDef;
            def+=charCode(k.key());
            def+=QString(",\"offset\":")+QString().number(k.value())+QString("}");
            kernings+=QString("\t\t")+def+QString(",\n");
        }
    }
    if (kernings.length()>2) {
        res+=QString("\t\"kernings\":[\n");
        res+=kernings;
        res+=QString("\t]\n");
    }
    res+=QString("};\n");
    out = res.toUtf8();
    return true;
}


AbstractExporter* JsExporterFactoryFunc (QObject* parent) {
    return new JsExporter(parent);
}
