#ifndef SCOPETRANSFERBASIC_H
#define SCOPETRANSFERBASIC_H

#include <atomic>
#include "transfer_global.h"

enum ScopeTransferStatus
{
    Ok = 0,
    Error = -1
};

class ScopeTransferBasic
{
public:
    // Constructor
    ScopeTransferBasic() = default;
    //Destructor
    virtual ~ScopeTransferBasic();
    bool isOpen() const { return m_open; }
    virtual ScopeTransferStatus open() = 0;
    virtual ScopeTransferStatus close() = 0;

protected:


protected:
    std::atomic<bool> m_open {false};
};

#endif // SCOPETRANSFERBASIC_H
