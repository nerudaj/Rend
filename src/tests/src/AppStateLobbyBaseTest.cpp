#include <app/AppStateLobbyBase.hpp>
#include <catch.hpp>

std::vector<PlayerOptions> adjustTeams(std::vector<PlayerOptions> opts)
{
    AppStateLobbyBase::adjustTeams(opts);
    return opts;
}

int getTeamRatio(const std::vector<PlayerOptions>& opts)
{
    int sum = 0;
    for (const auto& opt : opts)
    {
        REQUIRE(opt.team != Team::None);
        if (opt.team == Team::Red)
            --sum;
        else if (opt.team == Team::Blue)
            ++sum;
    }
    return sum;
}

TEST_CASE("[AppStateLobbyBase]")
{
    SECTION("adjustTeams")
    {
        SECTION("Works for 1 player")
        {
            REQUIRE(
                adjustTeams({ { .team = Team::None } }).front().team
                == Team::Blue);

            REQUIRE(
                adjustTeams({ { .team = Team::Red } }).front().team
                == Team::Red);

            REQUIRE(
                adjustTeams({ { .team = Team::Blue } }).front().team
                == Team::Blue);
        }

        SECTION("Works for 2 players")
        {
            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::None }, { .team = Team::None } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::Red }, { .team = Team::None } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::Red }, { .team = Team::Red } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::Red }, { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::Blue }, { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams(
                    { { .team = Team::None }, { .team = Team::Blue } }))
                == 0);
        }

        SECTION("Works for 3 players")
        {
            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::None } }))
                == 1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Red } }))
                == -1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Blue },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue } }))
                == 1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Blue } }))
                == -1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue } }))
                == 1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue } }))
                == 1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::Blue } }))
                == 1);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::Red },
                                           { .team = Team::Blue } }))
                == 1);
        }

        SECTION("Works for 4 players")
        {
            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::None } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Red } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Blue },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue },
                                           { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Blue },
                                           { .team = Team::Blue },
                                           { .team = Team::None },
                                           { .team = Team::Blue } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Red },
                                           { .team = Team::Red },
                                           { .team = Team::None },
                                           { .team = Team::Red } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::Red } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Blue },
                                           { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::None } }))
                == 0);

            REQUIRE(
                getTeamRatio(adjustTeams({ { .team = Team::Blue },
                                           { .team = Team::None },
                                           { .team = Team::None },
                                           { .team = Team::Red } }))
                == 0);
        }
    }
}