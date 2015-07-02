#include "coffeeneuralnet.h"

CoffeeNeuralNet::CoffeeNeuralNet(QObject *parent) : QObject(parent)
{
}

QObjectList CoffeeNeuralNet::getNeurons() const
{
    QObjectList l;
    for(CoffeeNeuron* n : m_neurons)
        l.append(n);
    return l;
}

void CoffeeNeuralNet::createNeuron()
{
    m_neurons.append(new CoffeeNeuron(this));
}

CoffeeNeuron::CoffeeNeuron(QObject *parent) : QObject(parent)
{
}

CoffeeNeuron::NeuronType CoffeeNeuron::type() const
{
    return m_type;
}

void CoffeeNeuron::badify(CoffeeNeuron *n, double d)
{
    if(!m_connections.contains(n))
        return;
    double c_w = m_connections.value(n);
    c_w += d;
    m_connections.insert(n,c_w);
}

void CoffeeNeuron::increaseThreshold(double d)
{
    threshold += d;
}

void CoffeeNeuron::activate()
{
    for(CoffeeNeuron* n : m_connections.keys())
        if(n!=this&&m_connections.value(n)<=threshold)
            n->activate();
    output();
}

void CoffeeNeuron::addOutput(CoffeeNeuron *n)
{
    m_connections.insert(n,1.0);
}

void CoffeeNeuron::setType(CoffeeNeuron::NeuronType type)
{
    m_type = type;
}
