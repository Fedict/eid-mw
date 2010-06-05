/****************************************************************************
** Meta object code from reading C++ file 'dlgwndmodal.h'
**
** Created: Sat Jun 5 12:34:55 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "dlgwndmodal.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'dlgwndmodal.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_dlgWndModal[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       6,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      13,   12,   12,   12, 0x08,
      32,   12,   12,   12, 0x08,
      55,   12,   12,   12, 0x08,
      77,   12,   12,   12, 0x08,
      97,   12,   12,   12, 0x08,
     116,   12,   12,   12, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_dlgWndModal[] = {
    "dlgWndModal\0\0on_btnOk_clicked()\0"
    "on_btnCancel_clicked()\0on_btnRetry_clicked()\0"
    "on_btnYes_clicked()\0on_btnNo_clicked()\0"
    "reject()\0"
};

const QMetaObject dlgWndModal::staticMetaObject = {
    { &dlgWndBase::staticMetaObject, qt_meta_stringdata_dlgWndModal,
      qt_meta_data_dlgWndModal, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &dlgWndModal::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *dlgWndModal::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *dlgWndModal::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_dlgWndModal))
        return static_cast<void*>(const_cast< dlgWndModal*>(this));
    return dlgWndBase::qt_metacast(_clname);
}

int dlgWndModal::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = dlgWndBase::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_btnOk_clicked(); break;
        case 1: on_btnCancel_clicked(); break;
        case 2: on_btnRetry_clicked(); break;
        case 3: on_btnYes_clicked(); break;
        case 4: on_btnNo_clicked(); break;
        case 5: reject(); break;
        default: ;
        }
        _id -= 6;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
