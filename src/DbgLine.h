#ifndef DBG_LINE_H
#define DBG_LINE_H

#include <deque>
#include <vector>

class DbgLine {
private:
    /* data */
    int next;
    char action;
    std::vector<int> stack;
public:
    DbgLine(int next, char action, std::deque<int>& stack) {
        this->next = next;
        this->action = action;
        this->stack.insert(this->stack.begin(), stack.begin(), stack.end());
    }
private:
    // 串行化
    friend class cereal::access;

    template<class Archive>
    void serialize(Archive &ar)
    {
        // serialize things by passing them to the archive
        ar( cereal::make_nvp("next", next),
            cereal::make_nvp("action", action),
            CEREAL_NVP(stack));
    }
};



#endif /* end of include guard: DBG_LINE_H */
