#include "client.hpp"
void Sendflie_client(TaskSocket file_socket, string curuid, string recvuid, int SENDFILE)
{
    string filepath;
    cout << "请输入要发送文件的路径" << endl;
    getline(cin, filepath);
    int filefd = open(filepath.c_str(), O_RDONLY);
    if (filefd == -1)
    {
        cerr << "输入的文件路径有误" << endl;
        return;
    }
    else
    {
        off_t filesize = lseek(filefd, 0, SEEK_END);
        lseek(filefd, 0, SEEK_SET);
        size_t lastSlash = filepath.find_last_of("/");
        string filename = filepath.substr(lastSlash + 1); // 提取文件名

        Message msg_file(curuid, recvuid, SENDFILE, {filename, to_string(filesize)});
        int ret = file_socket.Send(msg_file.S_to_json());
        err.server_close(ret);

        string recv_file = file_socket.Receive_client();
        err.server_close(recv_file);
        if (recv_file == "success")
        {
            ssize_t offset = 0;
            cout << filesize << endl;
            while (offset < filesize)
            {
                ssize_t sent_bytes = sendfile(file_socket.getfd(), filefd, &offset, filesize - offset);
                // cout<<sent_bytes<<endl;
                if (sent_bytes == -1)
                {
                    if (errno == EAGAIN || errno == EWOULDBLOCK)
                    {
                        continue; // 如果是阻塞错误码重试
                    }
                    else
                    {
                        cerr << "Error sending file: " << strerror(errno) << endl;
                        close(filefd);
                        break;
                    }
                }
                else if (sent_bytes == 0)
                {
                    cerr << "Connection closed while sending file." << endl;
                    break;
                }
            }
        }
        close(filefd);
    }
    string recv = asocket.Receive_client();
    err.server_close(recv);
    if (recv == "success")
    {
        cout << "文件上传成功" << endl;
    }
}
void Receiveflie_client(TaskSocket asocket, string curuid, string recvuid, int RECVFILE)
{
    string filepath;
    cout << "请输入保存文件的路径（带上想接收文件名）:" << endl;
    getline(cin, filepath);

    size_t lastSlash = filepath.find_last_of("/");
    string filename = filepath.substr(lastSlash + 1); // 文件名

    Message msg_file(curuid, recvuid, RECVFILE, {filename});
    int ret = asocket.Send(msg_file.S_to_json());
    err.server_close(ret);

    string recv_file = asocket.Receive_client();
    err.server_close(recv_file);
    if (recv_file == "failure")
    {
        cout << "暂无未接收的文件" << endl;
        return;
    }
    else
    {
        int filefd = open(filepath.c_str(), O_APPEND | O_WRONLY | O_CREAT, S_IRWXU);
        if (filefd == -1)
        {
            cerr << "Error opening file:" << strerror(errno) << endl;
            return;
        }
        cout << recv_file.c_str() << endl;
        ssize_t size = stoll(recv_file.c_str()); // 文件大小
        cout << size << endl;
        char buf[BUFSIZ];
        ssize_t Recv_bytes = 0;
        lseek(filefd, 0, SEEK_SET);
        while (size > Recv_bytes)
        {
            cout << "size:" << size << endl;
            ssize_t read_byte = read(asocket.getfd(), buf, sizeof(buf));
            if (read_byte == -1)
            {
                if (errno == EAGAIN || errno == EWOULDBLOCK)
                {
                    continue;
                }
                else
                {
                    cerr << "Error reading file: " << strerror(errno) << endl;
                }
            }
            if (read_byte == 0)
            {
                cerr << "Connection closed by client" << endl;
                break;
            }
            if (size == Recv_bytes)
            {
                cout << "size:" << size << endl;
                cout << "totalRecvByte:" << Recv_bytes << endl;
                // cerr << "Connection closed by client" << endl;
                break;
            }
            ssize_t Writeb = write(filefd, buf, read_byte);
            if (Writeb == -1)
            {
                cerr << "Error writing to file" << endl;
                break;
            }
            Recv_bytes += read_byte;
            // cout<<"total_receive:"<<Recv_bytes<<endl;
            // cout << "byteRead:" << read_byte << endl;
            // cout << "bytewrite" << Writeb << endl;
        }
        if (Recv_bytes < size)
        {
            cerr << "File transmission incomplete" << endl;
        }
        close(filefd);
        cout << "文件接收完毕" << endl;
    }
}