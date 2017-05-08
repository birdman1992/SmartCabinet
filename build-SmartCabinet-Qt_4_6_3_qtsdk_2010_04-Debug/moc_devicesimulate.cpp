/****************************************************************************
** Meta object code from reading C++ file 'devicesimulate.h'
**
** Created: Mon May 8 11:01:12 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/Device/devicesimulate.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'devicesimulate.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_DeviceSimulate[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       5,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       2,       // signalCount

 // signals: signature, parameters, type, tag, flags
      16,   15,   15,   15, 0x05,
      47,   15,   15,   15, 0x05,

 // slots: signature, parameters, type, tag, flags
      76,   15,   15,   15, 0x0a,
     105,   15,   15,   15, 0x08,
     129,   15,   15,   15, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_DeviceSimulate[] = {
    "DeviceSimulate\0\0sendCardReaderData(QByteArray)\0"
    "sendCodeScanData(QByteArray)\0"
    "recvLockCtrlData(QByteArray)\0"
    "group_card_clicked(int)\0group_drug_clicked(int)\0"
};

const QMetaObject DeviceSimulate::staticMetaObject = {
    { &QWidget::staticMetaObject, qt_meta_stringdata_DeviceSimulate,
      qt_meta_data_DeviceSimulate, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &DeviceSimulate::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *DeviceSimulate::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *DeviceSimulate::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_DeviceSimulate))
        return static_cast<void*>(const_cast< DeviceSimulate*>(this));
    return QWidget::qt_metacast(_clname);
}

int DeviceSimulate::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QWidget::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: sendCardReaderData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 1: sendCodeScanData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: recvLockCtrlData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: group_card_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 4: group_drug_clicked((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 5;
    }
    return _id;
}

// SIGNAL 0
void DeviceSimulate::sendCardReaderData(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 0, _a);
}

// SIGNAL 1
void DeviceSimulate::sendCodeScanData(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}
QT_END_MOC_NAMESPACE
