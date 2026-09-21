#include <unistd.h>

#define MAX_BUFFER_SIZE 256

typedef struct {
        char tokens[MAX_BUFFER_SIZE];
        char * token_ptrs[MAX_BUFFER_SIZE];
        int token_len[MAX_BUFFER_SIZE];
        int count;
} Tokens;

int is_space(char * byte){
        return (byte == " ");
}

int main() {
    // Some constants to move later.
    char msg[] = "STARTING PROGRAM\n";
    char buffer[MAX_BUFFER_SIZE];

    // Write starting message and start loop
    write(1, msg, sizeof(msg) - 1);
    // Add loop later.
    // while(1):

    // Read user input till end of buffer.
    ssize_t bytes = read(0, buffer, sizeof(buffer) - 1);
    // If user inputed something, convert it to individual tokens
    if(bytes > 0){
        int i = 0; // Iterate through input bytes
        int j = 0; // Write to tokens array one byte at a time
        Tokens t;
        t.count = 0; // Start count

        while(i < bytes){
            if(!is_space(&buffer[i])){
                // Mark the start of token number = current count with a pointer to the first symbol
                t.token_ptrs[t.count] = &t.tokens[j];
                while(!is_space(&buffer[i])){
                    // Copy byte into tokens array.
                    t.tokens[j] = buffer[i];
                    t.token_len[t.count]++;
                    i++;
                    j++;
                }
                t.count++;
            }
            i++;
        }

        // I tried to print the first token in the array but I get Seg Fault :(
        // This is clearly not working. Update later. 
        write(1, t.token_ptrs[0], t.token_len[0]);

        // For every token in the array, write it into the terminal
        int c = 0;
        while(c < t.count){
                write(1, t.token_ptrs[c], t.token_len[c]);
                write(1, "\n", 1);
                c++;
        }
    }

    return 0;
}