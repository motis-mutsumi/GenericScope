#ifndef SCOPECONTROLTRANSFER_H
#define SCOPECONTROLTRANSFER_H

#include "scopetransferbasic.h"

class TRANSFER_EXPORT ScopeControlTransfer : virtual public ScopeTransferBasic
{
protected:
    ScopeControlTransfer() = default;
    ~ScopeControlTransfer();

    virtual ScopeTransferStatus readData(uint8_t *data, uint32_t read_len, uint8_t *cmd = nullptr, int write_len = 0);
};

#endif // SCOPECONTROLTRANSFER_H
