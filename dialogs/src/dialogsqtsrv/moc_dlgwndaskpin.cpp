/****************************************************************************
** Meta object code from reading C++ file 'dlgwndaskpin.h'
**
** Created: Fri Jun 4 17:32:05 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dlgwndaskpin.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgwndaskpin.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_dlgWndAskPIN[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      13,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      14,   13,   13,   13, 0x08,
      36,   13,   13,   13, 0x08,
      58,   13,   13,   13, 0x08,
      80,   13,   13,   13, 0x08,
     102,   13,   13,   13, 0x08,
     124,   13,   13,   13, 0x08,
     146,   13,   13,   13, 0x08,
     168,   13,   13,   13, 0x08,
     190,   13,   13,   13, 0x08,
     212,   13,   13,   13, 0x08,
     234,   13,   13,   13, 0x08,
     261,  256,   13,   13, 0x08,
     292,  256,   13,   13, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_dlgWndAskPIN[] = {
    "dlgWndAskPIN\0\0on_tbtNUM_0_clicked()\0"
    "on_tbtNUM_1_clicked()\0on_tbtNUM_2_clicked()\0"
    "on_tbtNUM_3_clicked()\0on_tbtNUM_4_clicked()\0"
    "on_tbtNUM_5_clicked()\0on_tbtNUM_6_clicked()\0"
    "on_tbtNUM_7_clicked()\0on_tbtNUM_8_clicked()\0"
    "on_tbtNUM_9_clicked()\0on_tbtClear_clicked()\0"
    "text\0on_txtPIN_textChanged(QString)\0"
    "on_txtPIN_2_textChanged(QString)\0"
};

const QMetaObject dlgWndAskPIN::staticMetaObject = {
    { &dlgWndBase::staticMetaObject, qt_meta_stringdata_dlgWndAskPIN,
      qt_meta_data_dlgWndAskPIN, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &dlgWndAskPIN::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *dlgWndAskPIN::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *dlgWndAskPIN::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dlgWndAskPIN))
        return static_cast<void*>(const_cast< dlgWndAskPIN*>(this));
    return dlgWndBase::qt_metacast(_clname);
}

int dlgWndAskPIN::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = dlgWndBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_tbtNUM_0_clicked(); break;
        case 1: on_tbtNUM_1_clicked(); break;
        case 2: on_tbtNUM_2_clicked(); break;
        case 3: on_tbtNUM_3_clicked(); break;
        case 4: on_tbtNUM_4_clicked(); break;
        case 5: on_tbtNUM_5_clicked(); break;
        case 6: on_tbtNUM_6_clicked(); break;
        case 7: on_tbtNUM_7_clicked(); break;
        case 8: on_tbtNUM_8_clicked(); break;
        case 9: on_tbtNUM_9_clicked(); break;
        case 10: on_tbtClear_clicked(); break;
        case 11: on_txtPIN_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 12: on_txtPIN_2_textChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 13;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
