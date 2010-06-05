/****************************************************************************
** Meta object code from reading C++ file 'dlgwndaskpins.h'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dlgwndaskpins.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgwndaskpins.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_dlgWndAskPINs[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      17,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x08,
      49,   14,   14,   14, 0x08,
      84,   14,   14,   14, 0x08,
     119,   14,   14,   14, 0x08,
     157,   14,   14,   14, 0x08,
     170,   14,   14,   14, 0x08,
     182,   14,   14,   14, 0x08,
     204,   14,   14,   14, 0x08,
     226,   14,   14,   14, 0x08,
     248,   14,   14,   14, 0x08,
     270,   14,   14,   14, 0x08,
     292,   14,   14,   14, 0x08,
     314,   14,   14,   14, 0x08,
     336,   14,   14,   14, 0x08,
     358,   14,   14,   14, 0x08,
     380,   14,   14,   14, 0x08,
     402,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_dlgWndAskPINs[] = {
    "dlgWndAskPINs\0\0on_txtOldPIN_textChanged(QString)\0"
    "on_txtNewPIN1_textChanged(QString)\0"
    "on_txtNewPIN2_textChanged(QString)\0"
    "on_txtPIN_Keypad_textChanged(QString)\0"
    "FinalCheck()\0NextField()\0on_tbtNUM_0_clicked()\0"
    "on_tbtNUM_1_clicked()\0on_tbtNUM_2_clicked()\0"
    "on_tbtNUM_3_clicked()\0on_tbtNUM_4_clicked()\0"
    "on_tbtNUM_5_clicked()\0on_tbtNUM_6_clicked()\0"
    "on_tbtNUM_7_clicked()\0on_tbtNUM_8_clicked()\0"
    "on_tbtNUM_9_clicked()\0on_tbtClear_clicked()\0"
};

const QMetaObject dlgWndAskPINs::staticMetaObject = {
    { &dlgWndBase::staticMetaObject, qt_meta_stringdata_dlgWndAskPINs,
      qt_meta_data_dlgWndAskPINs, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &dlgWndAskPINs::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *dlgWndAskPINs::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *dlgWndAskPINs::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dlgWndAskPINs))
        return static_cast<void*>(const_cast< dlgWndAskPINs*>(this));
    return dlgWndBase::qt_metacast(_clname);
}

int dlgWndAskPINs::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = dlgWndBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_txtOldPIN_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: on_txtNewPIN1_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 2: on_txtNewPIN2_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: on_txtPIN_Keypad_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: FinalCheck(); break;
        case 5: NextField(); break;
        case 6: on_tbtNUM_0_clicked(); break;
        case 7: on_tbtNUM_1_clicked(); break;
        case 8: on_tbtNUM_2_clicked(); break;
        case 9: on_tbtNUM_3_clicked(); break;
        case 10: on_tbtNUM_4_clicked(); break;
        case 11: on_tbtNUM_5_clicked(); break;
        case 12: on_tbtNUM_6_clicked(); break;
        case 13: on_tbtNUM_7_clicked(); break;
        case 14: on_tbtNUM_8_clicked(); break;
        case 15: on_tbtNUM_9_clicked(); break;
        case 16: on_tbtClear_clicked(); break;
        default: ;
        }
        _id -= 17;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
