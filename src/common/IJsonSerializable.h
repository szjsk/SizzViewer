#ifndef IJSONSERIALIZABLE_H
#define IJSONSERIALIZABLE_H

#include <QJsonObject>

// 인터페이스: JSON 직렬화/역직렬화를 위한 순수 가상 클래스
class IJsonSerializable
{
public:
    virtual ~IJsonSerializable() = default;
    virtual QJsonObject toJson() const = 0;
    virtual void fromJson(const QJsonObject &json) = 0;
    virtual QString getFileName() = 0;
};

#endif // IJSONSERIALIZABLE_H
