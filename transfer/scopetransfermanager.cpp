#include "scopetransfermanager.h"
#include "scopetransferbasic.h"
#include "tcp/scopetcp.h"
#include "uart/scopeuart.h"
#include "modbus/scopemodbus.h"
#include "udp/scopeudp.h"
#include <mutex>
#include <memory>
#include <unordered_map>

ScopeTransferManager::~ScopeTransferManager()
{
}

void ScopeTransferManager::createTransfer(TransferType type, PPScopeTransferBasic pp_transfer)
{
    if (!pp_transfer)
    {
        return;
    }
    switch (type)
    {
    case TransferType::Uart:
        *pp_transfer = new ScopeUart();
        break;
    case TransferType::Tcp:
        *pp_transfer = new ScopeTcp();
        break;
    case TransferType::ModBus:
        *pp_transfer = new ScopeModBus();
        break;
    case TransferType::Udp:
        *pp_transfer = new ScopeUdp();
        break;
    default:
        return; // 无效类型
    }
    return;
}

void ScopeTransferManager::destroyTransfer(PScopeTransferBasic p_transfer)
{
    if (!p_transfer)
    {
        return;
    }
    delete p_transfer;
    return;
}
