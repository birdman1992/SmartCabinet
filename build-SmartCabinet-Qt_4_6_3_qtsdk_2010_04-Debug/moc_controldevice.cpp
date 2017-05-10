/****************************************************************************
** Meta object code from reading C++ file 'controldevice.h'
**
** Created: Wed May 10 17:29:57 2017
**      by: The Qt Meta Object Compiler version 62 (Qt 4.6.3)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../SmartCabinet/Device/controldevice.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'controldevice.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 62
#error "This file was generated using the moc from 4.6.3. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_ControlDevice[] = {

 // content:
       4,       // revision
       0,       // classname
       0,    0, // classinfo
       7,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       4,       // signalCount

 // signals: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x05,
      35,   14,   14,   14, 0x05,
      60,   14,   14,   14, 0x05,
      87,   14,   14,   14, 0x05,

 // slots: signature, parameters, type, tag, flags
     112,   14,   14,   14, 0x08,
     141,   14,   14,   14, 0x08,
     172,   14,   14,   14, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_ControlDevice[] = {
    "ControlDevice\0\0cardReaderTimeout()\0"
    "lockCtrlData(QByteArray)\0"
    "cardReaderData(QByteArray)\0"
    "codeScanData(QByteArray)\0"
    "readLockCtrlData(QByteArray)\0"
    "readCardReaderData(QByteArray)\0"
    "readCodeScanData(QByteArray)\0"
};

const QMetaObject ControlDevice::staticMetaObject = {
    { &QObject::staticMetaObject, qt_meta_stringdata_ControlDevice,
      qt_meta_data_ControlDevice, 0 }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &ControlDevice::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *ControlDevice::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *ControlDevice::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_ControlDevice))
        return static_cast<void*>(const_cast< ControlDevice*>(this));
    return QObject::qt_metacast(_clname);
}

int ControlDevice::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QObject::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        switch (_id) {
        case 0: cardReaderTimeout(); break;
        case 1: lockCtrlData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 2: cardReaderData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 3: codeScanData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 4: readLockCtrlData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 5: readCardReaderData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        case 6: readCodeScanData((*reinterpret_cast< QByteArray(*)>(_a[1]))); break;
        default: ;
        }
        _id -= 7;
    }
    return _id;
}

// SIGNAL 0
void ControlDevice::cardReaderTimeout()
{
    QMetaObject::activate(this, &staticMetaObject, 0, 0);
}

// SIGNAL 1
void ControlDevice::lockCtrlData(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 1, _a);
}

// SIGNAL 2
void ControlDevice::cardReaderData(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 2, _a);
}

// SIGNAL 3
void ControlDevice::codeScanData(QByteArray _t1)
{
    void *_a[] = { 0, const_cast<void*>(reinterpret_cast<const void*>(&_t1)) };
    QMetaObject::activate(this, &staticMetaObject, 3, _a);
}
QT_END_MOC_NAMESPACE
