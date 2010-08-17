/****************************************************************************
** Meta object code from reading C++ file 'edtgui.h'
**
** Created: Fri Apr 30 09:37:05 2010
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.2)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "edtgui.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'edtgui.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.2. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_eDTGui[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
      25,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
       8,    7,    7,    7, 0x08,
      31,    7,    7,    7, 0x08,
      58,    7,    7,    7, 0x08,
      87,    7,    7,    7, 0x08,
     122,    7,    7,    7, 0x08,
     159,    7,    7,    7, 0x08,
     181,    7,    7,    7, 0x08,
     202,    7,    7,    7, 0x08,
     223,    7,    7,    7, 0x08,
     248,    7,    7,    7, 0x08,
     271,    7,    7,    7, 0x08,
     296,    7,    7,    7, 0x08,
     323,    7,    7,    7, 0x08,
     348,    7,    7,    7, 0x08,
     369,    7,    7,    7, 0x08,
     393,    7,    7,    7, 0x08,
     419,    7,    7,    7, 0x08,
     443,    7,    7,    7, 0x08,
     468,    7,    7,    7, 0x08,
     495,    7,    7,    7, 0x08,
     522,    7,    7,    7, 0x08,
     549,    7,    7,    7, 0x08,
     583,  576,    7,    7, 0x08,
     604,    7,    7,    7, 0x08,
     620,    7,    7,    7, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_eDTGui[] = {
    "eDTGui\0\0on_pb_Cancel_clicked()\0"
    "on_pb_SaveReport_clicked()\0"
    "on_pb_SaveReport_2_clicked()\0"
    "on_pb_testAuthentication_clicked()\0"
    "on_stackedWidget_currentChanged(int)\0"
    "on_pb_Diag2_clicked()\0on_pb_Diag_clicked()\0"
    "on_pb_Quit_clicked()\0on_pb_SaveSend_clicked()\0"
    "on_pb_solved_clicked()\0on_pb_needhelp_clicked()\0"
    "on_pb_not_solved_clicked()\0"
    "on_pb_previous_clicked()\0on_pb_next_clicked()\0"
    "on_pb_Summary_clicked()\0"
    "on_pb_Summary_2_clicked()\0"
    "on_pb_Details_clicked()\0"
    "on_pb_Continue_clicked()\0"
    "on_pb_Continue_2_clicked()\0"
    "on_pb_Continue_3_clicked()\0"
    "on_pb_Analyse_fr_clicked()\0"
    "on_pb_Analyse_nl_clicked()\0pEvent\0"
    "customEvent(QEvent*)\0checkProgress()\0"
    "checkHeartbeat()\0"
};

const QMetaObject eDTGui::staticMetaObject = {
    { &QMainWindow::staticMetaObject, qt_meta_stringdata_eDTGui,
      qt_meta_data_eDTGui, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &eDTGui::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *eDTGui::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *eDTGui::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_eDTGui))
        return static_cast<void*>(const_cast< eDTGui*>(this));
    return QMainWindow::qt_metacast(_clname);
}

int eDTGui::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QMainWindow::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: on_pb_Cancel_clicked(); break;
        case 1: on_pb_SaveReport_clicked(); break;
        case 2: on_pb_SaveReport_2_clicked(); break;
        case 3: on_pb_testAuthentication_clicked(); break;
        case 4: on_stackedWidget_currentChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: on_pb_Diag2_clicked(); break;
        case 6: on_pb_Diag_clicked(); break;
        case 7: on_pb_Quit_clicked(); break;
        case 8: on_pb_SaveSend_clicked(); break;
        case 9: on_pb_solved_clicked(); break;
        case 10: on_pb_needhelp_clicked(); break;
        case 11: on_pb_not_solved_clicked(); break;
        case 12: on_pb_previous_clicked(); break;
        case 13: on_pb_next_clicked(); break;
        case 14: on_pb_Summary_clicked(); break;
        case 15: on_pb_Summary_2_clicked(); break;
        case 16: on_pb_Details_clicked(); break;
        case 17: on_pb_Continue_clicked(); break;
        case 18: on_pb_Continue_2_clicked(); break;
        case 19: on_pb_Continue_3_clicked(); break;
        case 20: on_pb_Analyse_fr_clicked(); break;
        case 21: on_pb_Analyse_nl_clicked(); break;
        case 22: customEvent((*reinterpret_cast< QEvent*(*)>(_a[1]))); break;
        case 23: checkProgress(); break;
        case 24: checkHeartbeat(); break;
        default: ;
        }
        _id -= 25;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
