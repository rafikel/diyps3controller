#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <Configuration.h>

#define MAX_CONFIGURATIONS 8

class Controller
{
    public:
        Controller();
        virtual ~Controller();
        Controller(const Controller& other);
        Controller& operator=(const Controller& other);
        Configuration* GetConfiguration(unsigned int i) { return m_Configurations+i; }
        void SetConfiguration(Configuration val, unsigned int i) { m_Configurations[i] = val; }
    protected:
    private:
        Configuration m_Configurations[MAX_CONFIGURATIONS];
};

#endif // CONTROLLER_H
