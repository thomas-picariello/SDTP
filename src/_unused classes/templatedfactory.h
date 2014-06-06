#ifndef TEMPLATEDFACTORY_H
#define TEMPLATEDFACTORY_H

#include <QMap>

template<typename BaseType, typename Key>
class TemplatedFactory
{
public:
    template <typename BaseType>
    class AbstractCreator
    {
    public:
        virtual ~AbstractCreator(){}
        virtual BaseType* create() = 0;
    };

    template <typename DerivedType, typename BaseType>
    class DerivedCreator : public AbstractCreator<BaseType>
    {
    public:
        virtual BaseType* create(){ return new DerivedType(); }
    };

    void reg(Key id, AbstractCreator<BaseType>* fn){ mFunctionMap[id] = fn; }
    BaseType* create(Key id){ return mFunctionMap[id]->create(); }

    ~TemplatedFactory(){ qDeleteAll(mFunctionMap); }

private:
    QMap<Key, AbstractCreator<BaseType>*> mFunctionMap;
};

#endif // TEMPLATEDFACTORY_H
