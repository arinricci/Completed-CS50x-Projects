#include <cs50.h>
#include <stdio.h>

#include <string.h>

// Max number of candidates
#define MAX 9

// preferences[i][j] is number of voters who prefer i over j
int preferences[MAX][MAX];

// locked[i][j] means i is locked in over j
bool locked[MAX][MAX];

// Each pair has a winner, loser
typedef struct
{
    int winner;
    int loser;
} pair;

// Array of candidates
string candidates[MAX];
pair pairs[MAX * (MAX - 1) / 2];

int pair_count;
int candidate_count;

// Function prototypes
bool vote(int rank, string name, int ranks[]);
void record_preferences(int ranks[]);
void add_pairs(void);
void sort_pairs(void);
void lock_pairs(void);
void print_winner(void);

bool lock_check(int winner, int loser);

int main(int argc, string argv[])
{
    // Check for invalid usage
    if (argc < 2)
    {
        printf("Usage: tideman [candidate ...]\n");
        return 1;
    }

    // Populate array of candidates
    candidate_count = argc - 1;
    if (candidate_count > MAX)
    {
        printf("Maximum number of candidates is %i\n", MAX);
        return 2;
    }
    for (int i = 0; i < candidate_count; i++)
    {
        candidates[i] = argv[i + 1];
    }

    // Clear graph of locked in pairs
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            locked[i][j] = false;
        }
    }

    pair_count = 0;
    int voter_count = get_int("Number of voters: ");

    // Query for votes
    for (int i = 0; i < voter_count; i++)
    {
        // ranks[i] is voter's ith preference
        int ranks[candidate_count];

        // Query for each rank
        for (int j = 0; j < candidate_count; j++)
        {
            string name = get_string("Rank %i: ", j + 1);

            if (!vote(j, name, ranks))
            {
                printf("Invalid vote.\n");
                return 3;
            }
        }

        record_preferences(ranks);

        printf("\n");
    }

    add_pairs();
    sort_pairs();
    lock_pairs();
    print_winner();
    return 0;
}

// Update ranks given a new vote
bool vote(int rank, string name, int ranks[])
{
    // look for candidate called name in candidates array
    for (int i = 0; i < candidate_count; i++)
    {
        // if candidate found, update ranks array
        if (strcmp(name, candidates[i]) == 0)
        {
            ranks[rank] = i;
            return true;
        }
    }

    // if no candidate found
    return false;
}

// Update preferences given one voter's ranks
void record_preferences(int ranks[])
{
    // sift through each spot in ranks, except the last since it is not preferred over any other candidate
    for (int r = 0; r < candidate_count - 1; r++)
    {
        // find the candidate at rank[r]
        for (int i = 0; i < candidate_count; i++)
        {
            if (ranks[r] == i)
            {
                // update preferences to show this candidate is preferred over the remaining ranked candidates
                for (int j = 1; j < candidate_count - r; j++)
                {
                    preferences[i][ranks[r + j]]++;
                }
            }
        }
    }
    return;
}

// Record pairs of candidates where one is preferred over the other
void add_pairs(void)
{
    // initialize pair_count
    pair_count = 0;

    // find pairs within the preferences matrix
    for (int i = 0; i < candidate_count; i++)
    {
        for (int j = 0; j < candidate_count; j++)
        {
            // when pair is found, create pair, add to new pair to pairs[] and update pair_count
            if (preferences[i][j] > preferences[j][i])
            {
                pairs[pair_count].winner = i;
                pairs[pair_count].loser = j;
                pair_count++;
            }
        }
    }
    return;
}

// Sort pairs in decreasing order by strength of victory
void sort_pairs(void)
{
    int swap;

    // bubble sort the smallest wins to the end of pairs array
    do
    {
        // initialize and re-initialize swap counter
        swap = 0;

        for (int i = 0; i < pair_count - 1; i++)
        {
            // sort pairs based on decreasing magnitude of win
            if (preferences[pairs[i].winner][pairs[i].loser] -
                    preferences[pairs[i].loser][pairs[i].winner] <
                preferences[pairs[i + 1].winner][pairs[i + 1].loser] -
                    preferences[pairs[i + 1].loser][pairs[i + 1].winner])
            {
                // swap pairs
                pair temp = pairs[i];
                pairs[i] = pairs[i + 1];
                pairs[i + 1] = temp;

                // add to swap counter
                swap++;
            }
        }
    }
    while (swap != 0); // end when no swaps are made

    return;
}

// Lock pairs into the candidate graph in order, without creating cycles
void lock_pairs(void)
{
    // top of ordered pairs list becomes the first locked pair
    locked[pairs[0].winner][pairs[0].loser] = true;

    for (int i = 1; i < pair_count; i++)
    {
        if (lock_check(pairs[i].winner, pairs[i].loser) == true)
        {
            // lock in next pair if it WON'T create cycle
            locked[pairs[i].winner][pairs[i].loser] = true;
        }
    }

    return;
}

bool lock_check(int winner, int loser)
{
    // base case: potential direct path from current loser to current winner
    for (int i = 0; i < candidate_count; i++)
    {
        // go into pair.loser's row in locked[], check for wins
        if (locked[loser][i] == true)
        {
            // if pair.loser has wins, check if the candidate they win over has won over pairs.winner
            if (locked[i][winner] == true)
            {
                // if they have won, return false as this would create a cycle
                return false;
            }
            // recursive case: additional levels required to find potential path to current winner
            else
            {
                if (lock_check(winner, i) == false)
                {
                    return false;
                }
            }
        }
    }

    // if no cycle is found, return true
    return true;
}

// Print the winner of the election
void print_winner(void)
{
    // initialize winner
    int winner;

    // look through each j-column in locked[]
    for (int j = 0; j < candidate_count; j++)
    {
        int t_count = 0;

        // check i-values of a candidate's j-colunm for wins (true values)
        for (int i = 0; i < candidate_count; i++)
        {
            if (locked[i][j] == true)
            {
                t_count++;
            }
        }

        // if all elements in j-column are false (no wins over them), we have a winner!
        if (t_count == 0)
        {
            winner = j;
        }
    }

    printf("%s\n", candidates[winner]);

    return;
}
