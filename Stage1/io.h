#ifndef IO_H
#define IO_H

#include <stddef.h>

#define WRITE_OUT(...) \
    write_out((const char *[]){ __VA_ARGS__ }, \
              sizeof((const char *[]){ __VA_ARGS__ }) / sizeof(const char *))

#define WRITE_ERR(...) \
    write_error((const char *[]){ __VA_ARGS__ }, \
              sizeof((const char *[]){ __VA_ARGS__ }) / sizeof(const char *))

/**
 * @brief Writes one or more messages to STDERR.
 *
 * The messages are concatenated into an internal fixed-size buffer
 * (256 bytes) and flushed with write(). Partial writes and EINTR
 * are handled internally, and errno is preserved across the call.
 *
 * @param messages Array of null-terminated strings. NULL entries are skipped.
 * @param count    Number of entries in @p messages.
 *
 * @return 0 on success, -1 if a write failed (errno is NOT modified).
 *
 * @warning Atomicity is only guaranteed while the total length of all
 *          messages fits in the internal buffer. If it is longer, the output
 *          is flushed in several write() calls, and other processes writing
 *          to the same STDERR (e.g. children in a pipeline) may interleave
 *          between those chunks, producing tangled output.
 *
 *          Example (multiple pipeline stages each reporting an error):
 *
 *              MYSH >> /foo$ a | b | c | d | e | f
 *              ERROR MYSH >> 'c' Command not found
 *
 *              ERROR MYSH >> 'ERROR MYSH >> 'ERROR MYSH >> 'ERROR MYSH >> 'fbed' Command not found
 *              ' Command not found
 *              ' Command not found
 *              ' Command not found
 *              ERROR MYSH >> 'a' Command not found
 *
 *          To avoid this, keep each call's total length within the buffer
 *          size.
 */
int write_error(const char *const messages[], size_t count);

/**
 * @brief Writes one or more messages to STDOUT.
 *
 * The messages are concatenated into an internal fixed-size buffer
 * (256 bytes) and flushed with write(). Partial writes and EINTR
 * are handled internally, and errno is preserved across the call.
 *
 * @param messages Array of null-terminated strings. NULL entries are skipped.
 * @param count    Number of entries in @p messages.
 *
 * @return 0 on success, -1 if a write failed (errno is NOT modified).
 *
 * @warning Atomicity is only guaranteed while the total length of all
 *          messages fits in the internal buffer. If it is longer, the output
 *          is flushed in several write() calls, and other processes writing
 *          to the same STDOUT (e.g. children in a pipeline) may interleave
 *          between those chunks, producing tangled output.
 *
 *          Example (multiple pipeline stages each reporting an error):
 *
 *              MYSH >> /foo$ a | b | c | d | e | f
 *              ERROR MYSH >> 'c' Command not found
 *
 *              ERROR MYSH >> 'ERROR MYSH >> 'ERROR MYSH >> 'ERROR MYSH >> 'fbed' Command not found
 *              ' Command not found
 *              ' Command not found
 *              ' Command not found
 *              ERROR MYSH >> 'a' Command not found
 *
 *          To avoid this, keep each call's total length within the buffer
 *          size.
 */
int write_out(const char *const messages[], size_t count);

#endif