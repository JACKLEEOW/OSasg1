#ifndef SIGNALS_H
#define SIGNALS_H

/* --------------------------- FUNCTION PROTOTYPES -------------------------- */

/** 
 * @brief Initializes the shell signal handlers 
 * @return 0 on success, -1 on failure
 * 
 */
int init_shell_signals();

/** 
 * @brief Restores signal handlers to default
 * @return 0 on success, -1 on failure
 */
int restore_children_default_signals();

#endif