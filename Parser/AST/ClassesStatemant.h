//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_CLASSSTATEMENT_H
#define KID_CLASSSTATEMENT_H

#pragma once
#include "../../libs/Classes.h"
#include "Statement.h"

class ClassDefineStatement : public Statement
{
    ClassInfo info;
public:
    explicit ClassDefineStatement(ClassInfo info) : info(std::move(info)) {}
    void execute(Environment&) const override { Classes::define(info); }
};

#endif