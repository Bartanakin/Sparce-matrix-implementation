//
// Created by bartanakin on 11/29/24.
//

#pragma once
#include <iostream>
#include <vector>

template<typename T>
class RowQueue {
    struct ListElement {
        T value;
        unsigned int col;
        int prev;
        int next;
    };

public:
    RowQueue(
        unsigned int capacity
    ):
        capacity(capacity) {
        this->list.reserve(capacity);
    }

    void mergeDown() {
        int rightQueuePos = (-1) * (this->queueBeg.size() - 1) - 1;
        int leftQueuePos = (-1) * (this->queueBeg.size() - 2) - 1;

        int currRight = this->queueBeg[this->queueBeg.size() - 1];
        int currLeft = this->queueBeg[this->queueBeg.size() - 2];
        int prevLeft = this->queueBeg[this->queueBeg.size() - 2];
        while (currLeft != leftQueuePos && currRight != rightQueuePos) {
            if (this->list[currLeft].col < this->list[currRight].col) {
                prevLeft = currLeft;
                currLeft = this->list[currLeft].next;
            } else if (this->list[currLeft].col == this->list[currRight].col) {
                this->list[currLeft].value += this->list[currRight].value;
                auto toBeRemoved = currRight;
                currRight = this->list[currRight].next;
                if (currRight == this->list.size() - 1) {
                    currRight = toBeRemoved;
                }

                prevLeft = currLeft;
                currLeft = this->list[currLeft].next;
                this->moveLastTo(toBeRemoved);
            } else {
                auto nextRight = this->list[currRight].next;
                this->extract(currRight);
                this->insertAfter(currRight, this->list[currLeft].prev);
                currRight = nextRight;
            }
        }

        while (currRight != rightQueuePos) {
            auto nextRight = this->list[currRight].next;
            this->extract(currRight);
            this->insertAfter(currRight, prevLeft);
            prevLeft = currRight;
            currRight = nextRight;
        }

        this->queueBeg.pop_back();
    }

    void mergeAll() {
        while (this->queueBeg.size() > 1) {
            this->mergeDown();
        }
    }

    void push(
        T value,
        unsigned int col
    ) {
        if (this->list.empty()) {
            this->queueBeg.push_back(0);
            this->list.push_back(ListElement{value, col, -1, -1});

            return;
        }

        if (this->list.size() < this->capacity) {
            auto last = this->list.size() - 1;
            auto next = this->list[last].next;
            if (this->list[last].col == col && next < 0) {
                this->list[last].value += value;
            } else if (this->list[last].col < col && next < 0) {
                this->list.push_back(ListElement{value, col, last, next});
                this->list[last].next = last + 1;
            } else {
                auto newQueueBeg = (-1) * this->queueBeg.size() - 1;
                this->queueBeg.push_back(this->list.size());
                this->list.push_back(ListElement{value, col, newQueueBeg, newQueueBeg});
            }

            return;
        }

        while (this->list.size() == this->capacity) {
            if (this->queueBeg.size() <= 1) {
                this->capacity *= 2;
                this->list.reserve(this->capacity);
            } else {
                this->mergeDown();
            }
        }

        this->push(value, col);
    }

    const std::vector<ListElement>& getElements() const { return this->list; }

    size_t size() const { return this->list.size(); }

    void print() const {
        for (unsigned int i = 0; i < this->list.size(); i++) {
            std::cout << "(" << this->list[i].value << "," << this->list[i].col << ") " << this->list[i].prev << " " << this->list[i].next
                      << std::endl;
        }

        std::cout << "---" << std::endl;
        for (unsigned int i = 0; i < this->queueBeg.size(); i++) {
            std::cout << this->queueBeg[i] << " ";
        }

        std::cout << std::endl;
    }

    const std::vector<unsigned int>& getQueueBeg() const { return queueBeg; }

private:
    unsigned int capacity;
    std::vector<ListElement> list;
    std::vector<unsigned int> queueBeg;

    void insertAfter(
        int toBeInserted,
        int afterPosition
    ) {
        this->list[toBeInserted].prev = afterPosition;
        if (afterPosition < 0) {
            auto queueIndex = (-1) * afterPosition - 1;
            this->list[toBeInserted].next = this->queueBeg[queueIndex];
            this->queueBeg[queueIndex] = toBeInserted;
        } else {
            this->list[toBeInserted].next = this->list[afterPosition].next;
            this->list[afterPosition].next = toBeInserted;
        }

        if (this->list[toBeInserted].next >= 0) {
            this->list[this->list[toBeInserted].next].prev = toBeInserted;
        }
    }

    void extract(
        int toBeExtracted
    ) {
        if (this->list[toBeExtracted].next >= 0) {
            this->list[this->list[toBeExtracted].next].prev = this->list[toBeExtracted].prev;
        }

        if (this->list[toBeExtracted].prev >= 0) {
            this->list[this->list[toBeExtracted].prev].next = this->list[toBeExtracted].next;
        } else {
            this->queueBeg[(-1) * this->list[toBeExtracted].prev - 1] = this->list[toBeExtracted].next;
        }
    }

    void moveLastTo(
        int toBeRemoved
    ) {
        this->extract(toBeRemoved);

        auto last = this->list.size() - 1;
        if (last != toBeRemoved) {
            auto insertAfter = this->list[last].prev;
            this->extract(last);
            this->list[toBeRemoved] = this->list[last];
            this->insertAfter(toBeRemoved, insertAfter);
        }

        this->list.pop_back();
    }
};
