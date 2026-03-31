#ifndef SCOPETRANSFERMANAGER_H
#define SCOPETRANSFERMANAGER_H

#include "transfer_global.h"

class ScopeTransferBasic;
using PScopeTransferBasic = ScopeTransferBasic *;
using PPScopeTransferBasic = ScopeTransferBasic **;
class TRANSFER_EXPORT ScopeTransferManager
{
public:
    enum class TransferType
    {
        Uart,
        Tcp,
        ModBus,
        Udp
    };

private:
    ScopeTransferManager() = default;
    ~ScopeTransferManager();

public:
    // get transfer by type
    static void createTransfer(TransferType type, PPScopeTransferBasic);
    static void destoryTransfer(PScopeTransferBasic);

private:
};

#endif // SCOPETRANSFERMANAGER_H
