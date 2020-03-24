#include <catch2/catch.hpp>
#include "AlphabetModel.hpp"

#include <cstdlib>
#include <ctime>

#pragma warning( disable : 6237 6319 )

SCENARIO("AlphabeModel has default values", "[AlphabetModel]")
{
	GIVEN("AlphabetModel instance")
	{
		AlphabetModel model;

		THEN("default occurence of every symbol is at least 1")
		{
			for (size_t i = 1; i < TOTAL_NUMBER_OF_SYMBOLS; i++)
			{
				CHECK(model.frequencyBegin(i) > model.frequencyBegin(i - 1));
			}
		}
	}
}

SCENARIO("AlphabetModel can update frequencies")
{
	GIVEN("AlphabetModel instance")
	{
		AlphabetModel model;

		WHEN("model is updated")
		{
			srand((int)time(NULL));
			for (int i = 0; i < 100; i++)
			{
				unsigned char randByte = rand() % 256; // from 0 to 255
				model.update(randByte);
			}

			THEN("begin frequency is ascending") {
				for (size_t i = 1; i < TOTAL_NUMBER_OF_SYMBOLS; i++){
					CHECK(model.frequencyBegin(i) > model.frequencyBegin(i - 1));
				}
			}
			THEN("end frequency is ascending") {
				for (size_t i = 1; i < TOTAL_NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyEnd(i) > model.frequencyEnd(i - 1));
				}
			}
			THEN("for every symbol: begin < end frequency") {
				for (size_t i = 0; i < TOTAL_NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyBegin(i) < model.frequencyEnd(i));
				}
			}
			THEN("for every symbol: begin = end of previous symbol") {
				CHECK(model.frequencyBegin(0) == 0);
				for (size_t i = 1; i < TOTAL_NUMBER_OF_SYMBOLS; i++) {
					CHECK(model.frequencyBegin(i) == model.frequencyEnd(i-1));
				}
			}
		}
	}
}