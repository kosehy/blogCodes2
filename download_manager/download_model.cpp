#include "download_model.hpp"
#include "download_manager.hpp"

namespace dm{

namespace model{

namespace{

using tag_enum = download_item::tag;

}

download_model::download_model(QObject *parent) :
    QAbstractTableModel(parent),
    manager_(new net::download_manager(parent))
{    
}

bool download_model::
append(QUrl const &value, QString const &save_at,
       QString const &save_as)
{
    auto &ran = data_.get<random>();
    if(ran.emplace_back("Waiting", value).second){
        manager_->append(value, save_at, save_as);
        return insertRows(static_cast<int>(ran.size()),
                          1);
    }

    return false;
}

int download_model::
columnCount(const QModelIndex&) const
{
    return 4;
}

QVariant download_model::data(const QModelIndex &index,
                              int role) const
{
    if(!index.isValid()){
        return {};
    }

    switch(role){
    case Qt::DisplayRole:{
        auto const &set = data_.get<random>();
        switch(static_cast<tag_enum>(index.column())){
        case tag_enum::name:{
            return set[index.row()].name_;
        }
        case tag_enum::percent:{
            return set[index.row()].percent_;
        }
        case tag_enum::size:{
            return set[index.row()].size_;
        }
        case tag_enum::status:{
            return set[index.row()].status_;
        }
        default:
            return false;
        }
    }
    }

    return {};
}

QVariant download_model::
headerData(int section, Qt::Orientation orientation,
           int role) const
{
    if(role == Qt::DisplayRole){
        if(orientation == Qt::Horizontal){
            switch(static_cast<tag_enum>(section)){
            case tag_enum::name:{
                return "Name";
            }
            case tag_enum::percent:{
                return "Percent";
            }
            case tag_enum::size:{
                return "Size";
            }
            case tag_enum::status:{
                return "Status";
            }

            }
        }else if(orientation == Qt::Vertical){
            return section + 1;
        }
    }

    return {};
}

bool download_model::insertRows(int row, int count,
                                const QModelIndex&)
{
    beginInsertRows({}, row, row + count - 1);
    endInsertRows();

    return true;
}

bool download_model::removeRows(int row, int count,
                                const QModelIndex&)
{
    beginRemoveRows({}, row, row + count - 1);
    auto &ran = data_.get<random>();
    for(int i = 0; i != count; ++i){
        ran.erase(std::begin(ran) + row);
    }
    endRemoveRows();

    return true;
}

int download_model::rowCount(const QModelIndex&) const
{
    return static_cast<int>(data_.size());
}

bool download_model::
setData(const QModelIndex &index,
        const QVariant &value, int role)
{
    if(!index.isValid() || role == Qt::EditRole){
        return false;
    }

    auto &ran = data_.get<random>();
    auto it = std::begin(ran) + index.row();
    if(it == std::end(ran)){
        return {};
    }
    switch(static_cast<tag_enum>(index.column())){
    case tag_enum::name:{
        ran.modify(it,
                   [&](auto &e){e.name_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::percent:{
        ran.modify(it,
                   [&](auto &e){e.percent_ = value.toFloat();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::size:{
        ran.modify(it,
                   [&](auto &e){e.size_ = value.value<size_t>();});
        emit dataChanged(index, index);
        return true;
    }
    case tag_enum::status:{
        ran.modify(it,
                   [&](auto &e){e.status_ = value.toString();});
        emit dataChanged(index, index);
        return true;
    }
    default:
        return false;
    }

    return true;
}

}

}
