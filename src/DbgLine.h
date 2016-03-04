#ifndef DBG_LINE_H
#define DBG_LINE_H

#include <deque>
#include <vector>

class DbgLine {
private:
    /* data */
    int state;
    int next;
    char action;
    std::vector<int> stack;
    std::vector<int> nodestack;
public:
    DbgLine(int state, int next, char action, std::deque<int>& stack, std::deque<int>& nodestack) {
        this->state = state;
        this->next = next;
        this->action = action;
        this->stack.insert(this->stack.begin(), stack.begin(), stack.end());
        this->nodestack.insert(this->nodestack.begin(), nodestack.begin(), nodestack.end());
    }
private:
    // 串行化
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &ar)
    {
        // serialize things by passing them to the archive
        ar( CEREAL_NVP(state),
            CEREAL_NVP(next),
            CEREAL_NVP(action),
            CEREAL_NVP(stack),
            CEREAL_NVP(nodestack));
    }
};



#endif /* end of include guard: DBG_LINE_H */
