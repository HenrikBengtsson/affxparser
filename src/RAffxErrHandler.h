#ifndef RAFFXERRHANDLER_H
#define RAFFXERRHANDLER_H

#include <string>
#include "ErrHandler.h"
#include "Verbose.h"
#include "Except.h"

/**
 * @brief Basic R_affx exception-forwarding error handler
 */
class RAffxErrHandler : public ErrHandler {

  public:
    RAffxErrHandler(bool doThrow) { 
        m_Throw = doThrow;
    }

    virtual ~RAffxErrHandler() {}

    /** 
     * Forward the exception, presumably back to R code.
     * 
     * @param msg - about what went wrong.
     */
    virtual void handleError(const std::string &msg) {
        if(m_Throw) {
            throw Except(msg);
        } else {
            Verbose::warn(0, msg);
            exit(1);
        }
    }

  private:
    bool m_Throw;          ///< throw an exception or abort()?
};

#endif /* RAFFXERRHANDLER_H */
