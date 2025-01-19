#ifndef BBPATTERN_H
#define BBPATTERN_H
namespace mmfs
{
    class BBPattern
    {
        friend class BlinkBuzz;

    public:
        BBPattern();
        BBPattern(const BBPattern &other);
        BBPattern &operator=(const BBPattern &other);

        BBPattern(int duration, int times, int pause = -1);
        ~BBPattern();
        BBPattern &a(int duration);      // add a duration to the pattern
        BBPattern &a(BBPattern pattern); // append a pattern to the pattern
        BBPattern &r(int duration);      // rest at end of pattern for duration
    private:
        class BBPatternNode
        {
        public:
            BBPatternNode(int duration);
            int duration;
            BBPatternNode *next;
        };

        // linked list of actions
        BBPatternNode *head;
        BBPatternNode *tail;
    };
}
#endif // !BBPATTERN_H