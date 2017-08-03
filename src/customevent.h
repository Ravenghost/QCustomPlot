#ifndef CUSTOMEVENT_H
#define CUSTOMEVENT_H

class QCPTouchEvent
{
public:
    QCPTouchEvent() : m_accept(true) {}

    inline void accept() { m_accept = true; }
    inline void ignore() { m_accept = false; }
    inline void setAccepted(bool accepted) { m_accept = accepted; }
    inline bool isAccepted() const { return m_accept; }

private:
    bool m_accept;
};

#endif // CUSTOMEVENT_H
