#pragma once

#include <libjson/Json.h>
#include <libwidget/model/TableModel.h>

class DeviceModel : public TableModel
{
private:
    json::Value *_data = nullptr;

public:
    ~DeviceModel() override;

    int rows() override;

    int columns() override;

    String header(int column) override;

    Variant data(int row, int column) override;

    void update() override;
};
