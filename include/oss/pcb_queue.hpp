#pragma once
#include <functional>
#include <stdexcept>
#include "pcb.hpp"

namespace OSS
{
    class PCBQueue
    {
    private:
        class ListNode
        {
        public:
            PCB val_;
            ListNode *next = nullptr;

            inline ListNode(PCB val) : val_(val) {}
        };

        ListNode *head_ = nullptr;
        ListNode *tail_ = nullptr;

    public:
        PCBQueue() : head_(nullptr), tail_(nullptr) {}

        inline void enqueue(PCB val)
        {
            ListNode *new_node = new ListNode(val);

            // if (tail_ == nullptr) {
            //     tail_->next = new_node;
            //     tail_ = tail_->next;
            // } else {
            //     head_ = tail_ = new_node;
            // }
            if (tail_ == nullptr) {
                head_ = tail_ = new_node;
            } else {
                tail_->next = new_node;
                tail_ = new_node;
            }
        }


        inline PCB dequeue()
        {
            if (head_ == nullptr) {
                throw std::runtime_error("Queue is empty");
            }

            PCB pcb = head_->val_;

            ListNode *old_head = head_;
            head_ = old_head->next;

            if (head_ == nullptr) {
                tail_ = nullptr;
            } 

            // clean up
            delete old_head;

            return pcb;
        }

        inline bool isEmpty()
        {
            if (tail_ == nullptr && head_ == nullptr)
            {
                return true;
            }
            return false;
        }

        inline void traverse(std::function<void(PCB*)> processValue) {
            for (ListNode* node = head_; node != nullptr; node = node->next) {
                processValue(&node->val_);
            }
        }

        inline PCB peek() {
            if (head_ == nullptr) {
                throw std::runtime_error("Queue is empty");
            }

            return head_->val_;
        }
        inline ~PCBQueue() {
            while (head_ != nullptr) {
                ListNode* tmp = head_;
                head_ = head_->next;
                delete tmp;
            }
            tail_ = nullptr;
        }

        PCBQueue(const PCBQueue&) = delete;
        PCBQueue& operator=(const PCBQueue&) = delete;
    };
}