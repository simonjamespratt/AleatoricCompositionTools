#include "catch.hpp"
#include "Engine.hpp"
#include "UniformGenerator.hpp"

// TODO: This needs to be instantiated ONCE for ALL tests to use,
// ideally in the main.cpp of the tests
// It also needs to be handled by a smart pointer or Engine::Instance() needs to return a smart pointer
static actlib::RNG::Engine *engine = actlib::RNG::Engine::Instance();

SCENARIO("UniformGenerator")
{
    GIVEN("The class is instantiated correctly")
    {
        actlib::Numbers::UniformGenerator instance(engine->getEngine(), 1, 2);

        WHEN("A number is requested")
        {
            THEN("It should produce random numbers within provided the range")
            {
                // NB: This is a pseudo test, in that it is unlikely to be wrong, but is not guaranteed to be right!
                // In order to test this properly it would require understanding how to deterministically seed the RNG
                // or Mock things (which I don't think is possible)
                for (int i = 0; i < 100; i++) {
                    int number = instance.getNumber();
                    REQUIRE(number >= 1);
                    REQUIRE(number <= 2);
                }
            }
        }

        WHEN("The distribution is changed")
        {
            instance.setDistribution(2, 3);

            AND_WHEN("A number is requested")
            {
                THEN("It should produce random numbers within provided the range")
            {
                // NB: This is a pseudo test, in that it is unlikely to be wrong, but is not guaranteed to be right!
                // In order to test this properly it would require understanding how to deterministically seed the RNG
                // or Mock things (which I don't think is possible)
                for (int i = 0; i < 100; i++) {
                    int number = instance.getNumber();
                    REQUIRE(number >= 2);
                    REQUIRE(number <= 3);
                }
            }
            }
        }
    }
}
