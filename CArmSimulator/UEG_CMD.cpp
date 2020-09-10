#include <ctime>
#include <mutex>

#include "UEG_CMD.h"

Msg_t::Msg_t(const Type type_, const std::uint16_t uid_, const std::vector<std::uint8_t>& arg_)
    : head{ Msg_t::MSG_HEAD }
    , uid{ uid_ }
    , type{ type_ }
    , arg{ arg_ }
{
    length = Size();
}

Msg_t::Msg_t(const Type type_, const float arg_)
    : head{Msg_t::MSG_HEAD}
    , uid{Msg_t::GenUID()}
    , type{type_}
{
    arg.clear();
    uint8_t* ptr = (uint8_t*)(&arg_);
    arg.insert(std::end(arg), ptr, ptr + sizeof(arg_));
    length = Size();
}

bool Msg_t::ReadData(QTcpSocket* socketPtr)
{
    auto Read = [socketPtr](auto& obj)
    {
        if (socketPtr->bytesAvailable() >= sizeof(obj))
        {
            return (socketPtr->read((char*)(&obj), sizeof(obj)) > 0);
        }
        return false;
    };

    auto ReadArg = [socketPtr, this]()
    {
        arg.resize(length - sizeof(head) - sizeof(length) - sizeof(uid) - sizeof(type));
        if (socketPtr->bytesAvailable() >= arg.size())
        {
            const auto byteNum = socketPtr->read((char*)(arg.data()), arg.size());
            return (byteNum == arg.size());
        }
        return false;
    };

    return Read(head) && Read(length) && Read(uid) && Read(type) && ReadArg();
}

bool Msg_t::Valid() const
{
    return ((Msg_t::MSG_HEAD == head) && (Size() == length));
}

std::size_t Msg_t::Size() const
{
    return (sizeof(head) + sizeof(length) + sizeof(uid) + sizeof(type) + arg.size());
}

std::vector<std::uint8_t> Msg_t::ToVec() const
{
    std::vector<std::uint8_t> vec;

    auto Append = [&vec](const auto& value)
    {
        uint8_t* ptr = (uint8_t*)(&value);
        vec.insert(std::end(vec), ptr, ptr + sizeof(value));
    };

    Append(head);
    Append(length);
    Append(uid);
    Append(type);
    vec.insert(std::end(vec), std::begin(arg), std::end(arg));

    return vec;
}

std::uint16_t Msg_t::GenUID()
{
    static bool bInitRand = false;
    static std::uint16_t u16Last{};
    static std::mutex mtxRand;

    if (!bInitRand)
    {
        std::srand(std::time(nullptr));
    }

    {
        std::lock_guard<std::mutex> lg(mtxRand);
        std::uint16_t rnd = (rand() / double(RAND_MAX)) * ((1 << (sizeof(unsigned short) * 8)) - 1);
        if (rnd == u16Last) rnd = (rand() / double(RAND_MAX)) * ((1 << (sizeof(unsigned short) * 8)) - 1);
        u16Last = rnd;
    }

    return u16Last;
}
