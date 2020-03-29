#include <catch2/catch.hpp>
#include "AdaptiveModel.hpp"

#include <cstdlib>
#include <ctime>

#pragma warning( disable : 6237 6319 )

SCENARIO("AlphabeModel has default values", "[AdaptiveModel]")
{
	const size_t NUMBER_OF_SYMBOLS = 7;
	const size_t MAX_FREQUENCY = 100;


	GIVEN("AdaptiveModel instance")
	{
		AdaptiveModel model(NUMBER_OF_SYMBOLS, MAX_FREQUENCY);

		THEN("default occurence of every symbol is at least 1")
		{
			for (size_t i = 1; i < NUMBER_OF_SYMBOLS; i++)
			{
				CHECK(model.frequencyBegin(i) > model.frequencyBegin(i - 1));
			}
		}
	}
}

SCENARIO("AdaptiveModel can update frequencies")
{
	const size_t NUMBER_OF_SYMBOLS = 7;
	const size_t MAX_FREQUENCY = 20;

	GIVEN("AdaptiveModel instance")
	{
		AdaptiveModel model(NUMBER_OF_SYMBOLS, MAX_FREQUENCY);

		WHEN("model is updated")
		{
			srand((int)time(NULL));
			for (int i = 0; i < 30; i++)
			{
				unsigned char randByte = rand() % NUMBER_OF_SYMBOLS; // from 0 to 255
				model.update(randByte);
			}

			THEN("begin frequency is ascending") {
				for (size_t i = 1; i < NUMBER_OF_SYMBOLS; i++){
					CHECK(model.frequencyBegin(i) > model.frequencyBegin(i - 1));
				}
			}
			THEN("end frequency is ascending") {
				for (size_t i = 1; i < NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyEnd(i) > model.frequencyEnd(i - 1));
				}
			}
			THEN("for every symbol: begin < end frequency") {
				for (size_t i = 0; i < NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyBegin(i) < model.frequencyEnd(i));
				}
			}
			THEN("for every symbol: begin = end of previous symbol") {
				CHECK(model.frequencyBegin(0) == 0);
				for (size_t i = 1; i < NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyBegin(i) == model.frequencyEnd(i-1));
				}
			}
		}
		
	}
}

SCENARIO("AdaptiveModel changes frequencies and scales")
{
	const size_t NUMBER_OF_SYMBOLS = 4;
	const size_t MAX_FREQUENCY = 8;
	GIVEN("AdaptiveModel instance")
	{
		AdaptiveModel model(NUMBER_OF_SYMBOLS, MAX_FREQUENCY);
		CHECK(model.totalFrequency() == NUMBER_OF_SYMBOLS);
		
		WHEN("update one symbol's frequency")
		{
			size_t frequencyBefore;
			size_t frequencyAfter;
			THEN("total frequency of a model increses") {
				frequencyBefore = model.totalFrequency();
				model.update(0);
				frequencyAfter = model.totalFrequency();
				CHECK(frequencyBefore == frequencyAfter - 1);
			}
		}

		WHEN("updating symbol would potentially overflow max frequency")
		{
			model.update(0);
			model.update(0);
			model.update(0);
			model.update(1);
			CHECK(model.totalFrequency() == MAX_FREQUENCY);
			model.update(1);
			model.update(2);

			THEN("total frequency doesn't exceed maximum") {
				CHECK(model.totalFrequency() <= MAX_FREQUENCY);
			}
			THEN("all symbols have positive frequency") {
				for (size_t s = 0; s < model.size(); s++) {
					CHECK(model.frequencyBegin(s) < model.frequencyEnd(s));
				}
			}
		}
	}

}

SCENARIO("Cannot reference non-existent symbol in model")
{
	const size_t NUMBER_OF_SYMBOLS = 7;
	const size_t MAX_FREQUENCY = 100;


	GIVEN("AdaptiveModel instance")
	{
		AdaptiveModel model(NUMBER_OF_SYMBOLS, MAX_FREQUENCY);

		WHEN("try to UPDATE non-existent symbol")
		{
			THEN("an exception is thrown") {
				CHECK_THROWS(model.update(NUMBER_OF_SYMBOLS));
				CHECK_THROWS(model.update(NUMBER_OF_SYMBOLS + 1));
			}
		}
		WHEN("try to reference non-existent symbol")
		{
			THEN("an exception is thrown") {
				CHECK_THROWS(model.frequencyBegin(NUMBER_OF_SYMBOLS));
				CHECK_THROWS(model.frequencyEnd(-1));
			}
		}
	}
}