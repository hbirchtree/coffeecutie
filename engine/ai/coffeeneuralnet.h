#ifndef COFFEENEURALNET_H
#define COFFEENEURALNET_H

#include <QObject>
#include <QHash>
#include <QVariantMap>

class CoffeeNeuron : public QObject
{
    Q_PROPERTY(NeuronType type READ type WRITE setType)
    Q_OBJECT
public:
    enum NeuronType {
        Regular, //may have outputs, connects to other neurons
        Sensor //brings input into the neural network
    };
    Q_ENUMS(NeuronType)
    CoffeeNeuron(QObject* parent);

    NeuronType type() const;

signals:
    void output();
public slots:
    void badify(CoffeeNeuron* n, double d);
    void increaseThreshold(double d);
    void activate();
    void addOutput(CoffeeNeuron* n);
    void setType(NeuronType type);

private:
    QHash<CoffeeNeuron*,double> m_connections; //neuron,weight
    NeuronType m_type = Regular;
    double threshold = 1.0;
};

class CoffeeNeuralNet : public QObject
{
    Q_PROPERTY(QObjectList neurons READ getNeurons)
    Q_OBJECT
public:
    CoffeeNeuralNet(QObject *parent);
    QObjectList getNeurons() const;

public slots:
    void createNeuron();

private:
    QList<CoffeeNeuron*> m_neurons;
};

#endif // COFFEENEURALNET_H
