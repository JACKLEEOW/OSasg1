#ifndef IO_H
#define IO_H

/**
 * @brief Writes a message to error
 * @param message A null terminated string
 */
void write_error(const char *message);

/**
 * @brief Writes a message to out
 * @param message A null terminated string
 */
void write_out(const char *message);

#endif