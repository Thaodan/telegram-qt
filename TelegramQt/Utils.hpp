/*
   Copyright (C) 2014-2017 Alexandr Akulich <akulichalexander@gmail.com>

   This file is a part of TelegramQt library.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

 */

#ifndef UTILS_HPP
#define UTILS_HPP

#include <QByteArray>

#include "crypto-aes.hpp"
#include "TelegramNamespace.hpp"

namespace Telegram {

struct RsaPrivateKey {
    QByteArray modulus; // n
    QByteArray exponent; // e
    QByteArray d; // d, secret exponent
    QByteArray p; // p
    QByteArray q; // q
    quint64 fingersprint;
};

namespace Utils {

int randomBytes(QByteArray *array);
int randomBytes(quint64 *number);
int randomBytes(char *buffer, int count);
quint64 greatestCommonOddDivisor(quint64 a, quint64 b);
quint64 findDivider(quint64 number);
QByteArray sha1(const QByteArray &data);
QByteArray sha256(const QByteArray &data);
quint64 getFingersprint(const QByteArray &data, bool lowerOrderBits = true);
quint64 getRsaFingersprint(const Telegram::RsaKey &key);
Telegram::RsaKey loadHardcodedKey();
Telegram::RsaKey loadRsaKeyFromFile(const QString &fileName);
RsaPrivateKey loadRsaPrivateKeyFromFile2(const QString &fileName);
Telegram::RsaKey loadRsaPrivateKeyFromFile(const QString &fileName);
Telegram::RsaKey loadRsaKey();
QByteArray binaryNumberModExp(const QByteArray &data, const QByteArray &mod, const QByteArray &exp);
QByteArray rsa(const QByteArray &data, const Telegram::RsaKey &key);
QByteArray aesDecrypt(const QByteArray &data, const SAesKey &key);
QByteArray aesEncrypt(const QByteArray &data, const SAesKey &key);
QByteArray unpackGZip(const QByteArray &data);

}

inline int Utils::randomBytes(QByteArray *array)
{
    return randomBytes(array->data(), array->size());
}

inline int Utils::randomBytes(quint64 *number)
{
    return randomBytes((char *) number, 8);
}

inline QByteArray Utils::rsa(const QByteArray &data, const Telegram::RsaKey &key)
{
    return binaryNumberModExp(data, key.modulus, key.exponent);
}

} // Telegram

#endif // UTILS_HPP
