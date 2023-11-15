#include <catch.hpp>
#include "include/Shortcuts/ShortcutCombo.hpp"

TEST_CASE("[ShortcutCombo]")
{
	SECTION("Comparison")
	{
		SECTION("Lesser if ctrl differs")
		{
			ShortcutCombo combo1 = {
				.key = sf::Keyboard::A
			};
			ShortcutCombo combo2 = {
				.ctrlRequired = true,
				.key = sf::Keyboard::A
			};

			const bool cmpr = combo1 < combo2;
			REQUIRE(cmpr);
		}

		SECTION("Lesser if shift differs")
		{
			ShortcutCombo combo1 = {
				.key = sf::Keyboard::A
			};
			ShortcutCombo combo2 = {
				.shiftRequired = true,
				.key = sf::Keyboard::A
			};

			const bool cmpr = combo1 < combo2;
			REQUIRE(cmpr);
		}

		SECTION("Lesser/greater if key differs")
		{
			ShortcutCombo combo1 = {
				.key = sf::Keyboard::A
			};
			ShortcutCombo combo2 = {
				.key = sf::Keyboard::B
			};

			const bool cmpr = combo1 < combo2;
			REQUIRE(cmpr);
		}

		SECTION("Equal if everything equals")
		{
			ShortcutCombo combo = {
				.shiftRequired = true,
				.key = sf::Keyboard::End
			};

			const bool cmpr = combo <=> combo == 0;
			REQUIRE(cmpr);
		}

		SECTION("Mixed values")
		{
			ShortcutCombo combo1 = {
				.shiftRequired = true,
				.key = sf::Keyboard::M
			};
			ShortcutCombo combo2 = {
				.ctrlRequired = true,
				.key = sf::Keyboard::O
			};

			const bool cmpr1 = combo1 < combo2;
			const bool cmpr2 = combo2 < combo1;
			const bool cmpr3 = combo1 > combo2;
			REQUIRE(cmpr1);
			REQUIRE(!cmpr3);
			REQUIRE(!cmpr2);
		}

		SECTION("Mixed values")
		{
			ShortcutCombo combo1 = {
				.key = sf::Keyboard::Left
			};
			ShortcutCombo combo2 = {
				.shiftRequired = true,
				.key = sf::Keyboard::T
			};

			const bool cmpr1 = combo1 < combo2;
			const bool cmpr2 = combo2 < combo1;
			const bool cmpr3 = combo1 > combo2;
			REQUIRE(cmpr1);
			REQUIRE(!cmpr3);
			REQUIRE(!cmpr2);
		}
	}

	SECTION("Serialization")
	{
		SECTION("Only key")
		{
			ShortcutCombo combo = {
				.key = sf::Keyboard::A
			};
			auto str = std::to_string(combo);
			REQUIRE(str == "A");
		}

		SECTION("Ctr + key")
		{
			ShortcutCombo combo = {
				.ctrlRequired = true,
				.key = sf::Keyboard::H
			};
			auto str = std::to_string(combo);
			REQUIRE(str == "Ctrl+H");
		}

		SECTION("Shift + key")
		{
			ShortcutCombo combo = {
				.shiftRequired = true,
				.key = sf::Keyboard::Enter
			};
			auto str = std::to_string(combo);
			REQUIRE(str == "Shift+key58");
		}

		SECTION("Ctrl + Shift + Key")
		{
			ShortcutCombo combo = {
				.ctrlRequired = true,
				.shiftRequired = true,
				.key = sf::Keyboard::Z
			};
			auto str = std::to_string(combo);
			REQUIRE(str == "Ctrl+Shift+Z");
		}
	}
}