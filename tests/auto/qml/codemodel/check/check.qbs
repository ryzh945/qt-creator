import qbs
import "../../../autotest.qbs" as Autotest

Autotest {
    name: "QML code model check autotest"
    Depends { name: "LanguageUtils" }
    Depends { name: "QmlJS" }
    Depends { name: "Utils" }
    Depends { name: "Qt.widgets" }
    files: "tst_check.cpp"
    cpp.defines: base.concat([
        'QT_CREATOR',
        'QTCREATORDIR="' + project.ide_source_tree + '"',
        'TESTSRCDIR="' + path + '"'
    ])
}
