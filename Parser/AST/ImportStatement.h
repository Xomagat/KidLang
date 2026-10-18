//
// Created by Xomagat on 18.10.2026.
//

#ifndef KID_IMPORTSTATEMENT_H
#define KID_IMPORTSTATEMENT_H

#pragma once
#include "Statement.h"
#include "../../libs/NativeLibs.h"

class ImportStatement : public Statement
{
    std::string name;
public:
    explicit ImportStatement(std::string name) : name(std::move(name)) {}
    void execute(Environment& env) const override { NativeLibs::load(name, env); }
};

#endif // KID_IMPORTSTATEMENT_H
