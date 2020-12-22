#include <utility>

#include "solution.h"

using deck_of_cards = std::deque<unsigned int>;

enum class winner {
    Player1 = 0,
    Player2 = 1,
    Undecided = 2
};

enum class game_type {
    simple,
    recursive
};

const bool debug = false;

class game {
public:

    game(deck_of_cards deck_1, deck_of_cards deck_2, game_type type_) : type{type_} {
        deck[0] = std::move(deck_1);
        deck[1] = std::move(deck_2);
        game_id = next_game_id++;

        if (debug) {
            std::cout << "Initializing new game " << game_id << " with decks:" << std::endl;
            std::cout << "\t" << fmt::format("{}", fmt::join(deck[0], ",")) << std::endl;
            std::cout << "\t" << fmt::format("{}", fmt::join(deck[1], ",")) << std::endl;
        }
    }

    deck_of_cards get_deck(int which) {
        return deck[which];
    }

    winner play_round() {
        if (debug) std::cout << "Playing round " << round++ << std::endl;
        winner winner_of_this_round = winner::Undecided;

        size_t card_player_1, card_player_2;
        switch (type) {
            case game_type::simple: {
                if (debug) {
                    std::cout << "\tPlayer 1's deck: " << fmt::format("{}", fmt::join(deck[0], ",")) << std::endl;
                    std::cout << "\tPlayer 2's deck: " << fmt::format("{}", fmt::join(deck[1], ",")) << std::endl;
                }

                card_player_1 = deck[0].front();
                deck[0].pop_front();
                if (debug) std::cout << "\tPlayer 1 plays: " << card_player_1 << std::endl;

                card_player_2 = deck[1].front();
                deck[1].pop_front();
                if (debug) std::cout << "\tPlayer 2 plays: " << card_player_2 << std::endl;

                winner_of_this_round = card_player_1 > card_player_2 ? winner::Player1 : winner::Player2;

                break;
            }
            case game_type::recursive: {
                auto history_frame = fmt::format("{}::{}", fmt::join(deck[0], ","), fmt::join(deck[1], ","));
                if (history.count(history_frame)) {
                    // the game instantly ends in a win for player 1.
                    if (debug) std::cout << "This setup has been before. Player 1 wins." << std::endl;
                    return winner::Player1;
                } else {
                    history[history_frame] = true;

                    if (debug) {
                        std::cout << "\tPlayer 1's deck: " << fmt::format("{}", fmt::join(deck[0], ",")) << std::endl;
                        std::cout << "\tPlayer 2's deck: " << fmt::format("{}", fmt::join(deck[1], ",")) << std::endl;
                    }

                    card_player_1 = deck[0].front();
                    deck[0].pop_front();
                    if (debug) std::cout << "\tPlayer 1 plays: " << card_player_1 << std::endl;

                    card_player_2 = deck[1].front();
                    deck[1].pop_front();
                    if (debug) std::cout << "\tPlayer 2 plays: " << card_player_2 << std::endl;

                    if (deck[0].size() >= card_player_1 && deck[1].size() >= card_player_2) {
                        // trigger recursive subgame
                        if (debug) std::cout << "Playing a sub-game to determine the winner..." << std::endl;
                        auto subgame = game{deck_of_cards{deck[0].begin(), deck[0].begin() + card_player_1},
                                            deck_of_cards{deck[1].begin(), deck[1].begin() + card_player_2},
                                            game_type::recursive};
                        do {
                            winner_of_this_round = subgame.play_round();
                        } while (winner_of_this_round == winner::Undecided);
                        if (debug)
                            std::cout << "The winner of game " << game_id << " is player "
                                      << (winner_of_this_round == winner::Player1 ? "1" : "2") << "!" << std::endl;
                    } else {
                        winner_of_this_round = card_player_1 > card_player_2 ? winner::Player1 : winner::Player2;
                        if (debug)
                            std::cout << "\tPlayer " << (winner_of_this_round == winner::Player1 ? "1" : "2")
                                      << " wins round "
                                      << (round - 1) << " of game " << game_id << std::endl;
                    }
                }
                break;
            }
        }
        if (winner_of_this_round == winner::Player1) {
            deck[0].push_back(card_player_1);
            deck[0].push_back(card_player_2);
        } else {
            deck[1].push_back(card_player_2);
            deck[1].push_back(card_player_1);
        }

        if (deck[0].empty())
            return winner::Player2;
        else if (deck[1].empty())
            return winner::Player1;
        else
            return winner::Undecided;
    }

    game_type type;
    int game_id;
    static int next_game_id;
    deck_of_cards deck[2];
    std::unordered_map<std::string, bool> history{};
    unsigned int round{1};
};

int game::next_game_id = 1;

class day22 : public aoc::solution {
protected:
    static unsigned long part_1(deck_of_cards deck1, deck_of_cards deck2) {
        auto g = game{std::move(deck1), std::move(deck2), game_type::simple};
        winner game_winner = winner::Undecided;
        while (game_winner == winner::Undecided) {
            game_winner = g.play_round();
        }

        auto winner_as_int = static_cast<std::underlying_type<winner>::type>(game_winner);
        return get_deck_score(g.get_deck(winner_as_int));
    }

    static unsigned long part_2(deck_of_cards deck1, deck_of_cards deck2) {
        auto g = game{std::move(deck1), std::move(deck2), game_type::recursive};
        winner game_winner = winner::Undecided;
        while (game_winner == winner::Undecided) {
            game_winner = g.play_round();
        }

        auto winner_as_int = static_cast<std::underlying_type<winner>::type>(game_winner);
        return get_deck_score(g.get_deck(winner_as_int));
    }

    void run(std::istream& in, std::ostream& out) override {
        deck_of_cards deck[2];
        int current_deck = 0;
        for (std::string line; std::getline(in, line);) {
            line = aoc::trim(line);
            if (line == "Player 1:") {
                current_deck = 0;
            } else if (line == "Player 2:") {
                current_deck = 1;
            } else if (!line.empty()) {
                deck[current_deck].push_back(std::stoi(line));
            }
        }

        out << part_1(deck[0], deck[1]) << std::endl;
        out << part_2(deck[0], deck[1]) << std::endl;
    }

    static unsigned long get_deck_score(const deck_of_cards& deck) {
        unsigned int result{0};
        int multiplier = int(deck.size());
        for (auto& card : deck) {
            result += card * multiplier;
            multiplier--;
        }
        return result;
    }

    void register_tests() override {
        add_test(aoc::test{
                .name = "First",
                .input = "Player 1:\n"
                         "9\n"
                         "2\n"
                         "6\n"
                         "3\n"
                         "1\n"
                         "\n"
                         "Player 2:\n"
                         "5\n"
                         "8\n"
                         "4\n"
                         "7\n"
                         "10",
                .expected_output = "306\n291"
        });
    }
};

int main(int argc, char** argv) {
    day22 solution{};
    auto start = std::chrono::high_resolution_clock::now();
    auto exit_code = solution.execute(argc, argv);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "Taken " << duration.count() << " ms" << std::endl;
    return exit_code;
}
