#include "AdjacentSteps.hpp"

#include "ErrorChecker.hpp"

namespace aleatoric {
AdjacentSteps::AdjacentSteps(std::unique_ptr<IDiscreteGenerator> generator)
: m_generator(std::move(generator)),
  m_range(Range(0, 1)),
  m_haveRequestedFirstNumber(false)
{
    m_generator->setDistributionVector(m_range.size, 1.0);
}

AdjacentSteps::AdjacentSteps(std::unique_ptr<IDiscreteGenerator> generator,
                             Range range)
: m_generator(std::move(generator)),
  m_range(range),
  m_haveRequestedFirstNumber(false)
{
    m_generator->setDistributionVector(m_range.size, 1.0);
}

AdjacentSteps::~AdjacentSteps()
{}

int AdjacentSteps::getIntegerNumber()
{
    auto generatedNumber = m_generator->getNumber();
    m_lastReturnedNumber = generatedNumber + m_range.offset;

    m_haveRequestedFirstNumber = true;

    // step configuring in preparation for next call
    prepareStepBasedDistribution(m_lastReturnedNumber);

    return m_lastReturnedNumber;
}

double AdjacentSteps::getDecimalNumber()
{
    return static_cast<double>(getIntegerNumber());
}

void AdjacentSteps::setParams(NumberProtocolConfig newParams)
{
    m_range = newParams.getRange();
    m_generator->setDistributionVector(m_range.size, 1.0);

    if(m_haveRequestedFirstNumber &&
       m_range.numberIsInRange(m_lastReturnedNumber)) {
        prepareStepBasedDistribution(m_lastReturnedNumber);
    }
}

NumberProtocolConfig AdjacentSteps::getParams()
{
    return NumberProtocolConfig(m_range,
                                NumberProtocolParams(AdjacentStepsParams()));
}

void AdjacentSteps::prepareStepBasedDistribution(int number)
{
    auto vectorIndex = number - m_range.offset;
    m_generator->updateDistributionVector(0.0);

    if(number == m_range.start) {
        m_generator->updateDistributionVector(vectorIndex + 1, 1.0);

    } else if(number == m_range.end) {
        m_generator->updateDistributionVector(vectorIndex - 1, 1.0);

    } else {
        m_generator->updateDistributionVector(vectorIndex + 1, 1.0);
        m_generator->updateDistributionVector(vectorIndex - 1, 1.0);
    }
}
} // namespace aleatoric
