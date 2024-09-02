#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>


void FirstThreadInput(std::string& buffer, std::mutex& mtx, std::mutex& mtx2)
{

    mtx.lock();
    while (true) {
        mtx2.lock();
        std::string temporary_buffer;
        bool input_validation;
        // Проверка строки
        do {
            input_validation = false;
            std::cout << "Enter a new line: ";
            std::cin >> temporary_buffer;
            if (temporary_buffer.length() > 64) {
                input_validation = true;
                std::cout << "Error: Too long string. Length must be less than 65. Try entering the string again." << std::endl;
            }
            else {
                for (unsigned i = 0; temporary_buffer[i] != '\0'; i++) {
                    if (std::isdigit(temporary_buffer[i])) {

                    }
                    else {
                        input_validation = true;
                        std::cout << "Error: String must contain only digits. Try entering the string again." << std::endl;
                        break;
                    }
                }
            }
        } while (input_validation);
        mtx2.unlock();
        // Изменение строки по условию
        std::sort(temporary_buffer.begin(), temporary_buffer.end(), std::greater<char>());

        for (size_t i = 0; i < temporary_buffer.length(); ++i) {
            if ((temporary_buffer[i] - '0') % 2 == 0) {
                temporary_buffer[i] = 'K';
                temporary_buffer.insert(i + 1, "B");
                i++;
            }
        }
        // Занесение в буффер
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        mtx2.lock();
        buffer = temporary_buffer;
        std::cout << "Changed  line   : " << buffer << std::endl;
        mtx2.unlock();
        mtx.unlock();
        while (!buffer.empty()) {}
        mtx.lock();
    }
}

int main() {
    std::mutex wait_for_buffer;
    std::mutex wait_for_server;
    std::string buffer;
    std::thread First([&buffer, &wait_for_buffer, &wait_for_server]() {FirstThreadInput(buffer, wait_for_buffer, wait_for_server); });
    std::string data;
    int sock; 
    struct sockaddr_in addr; 



    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        wait_for_buffer.lock();
        data = buffer;
        buffer.clear();
        wait_for_buffer.unlock();
        unsigned summ_of_digits = 0;
        for (unsigned i = 0; data[i] != '\0'; i++) {
            if (std::isdigit(data[i])) {
                summ_of_digits += (data[i] - '0');
            }
            else {
                i++;
            }
        }
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            exit(1);
        }

        addr.sin_family = AF_INET;
        addr.sin_port = htons(6562);
        addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);


        if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
            wait_for_server.lock();
            while (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0)
            {

                perror("connect");
                std::cout << "Reconnection is in progress. Wait" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(5000));

            }
            wait_for_server.unlock();
        }
        send(sock, &summ_of_digits, sizeof(summ_of_digits), 0);
        close(sock);
    }
    First.join();
    return 0;
}