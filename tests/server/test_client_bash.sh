
#!/bin/bash

set -e

# Compile the clients
gcc -Wall -Wextra -g test_client_1.c  test_server_interface.c test_server_logic.c -o client_1.out
gcc -Wall -Wextra -g test_client_2.c  test_server_interface.c test_server_logic.c -o client_2.out
gcc -Wall -Wextra -g test_client_3.c  test_server_interface.c test_server_logic.c -o client_3.out
gcc -Wall -Wextra -g test_client_4.c  test_server_interface.c test_server_logic.c -o client_4.out

echo "Compilation completed successfully!"
