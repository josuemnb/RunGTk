#ifndef EVALUATOR_H
#define EVALUATOR_H

class Evaluator{

    private:
    Tokenizer token;


    public:
    void If();
    void For();
    void Puts();
    void Exit();
    void Read();
    void Var();
    void Const();
    void Next();
    void Block();
};

#endif // EVALUATOR_H_INCLUDED
