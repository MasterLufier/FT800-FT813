#include "ft8xxmemory.h"

#include <ft8xx.h>

using namespace EVE;

RamG::RamG(FT8xx * parent, uint32_t size = EVE_RAM_G_SAFETY_SIZE) :
    m_parent(parent)
{
    if(size > EVE_RAM_G_SIZE)
        error("Allocated size must be less than EVE_RAM_G_SIZE");
    debug_if(
        size > EVE_RAM_G_SAFETY_SIZE,
        "Note: If the loading image is in PNG format, the top 42K bytes from address "
        "0xF5800 of RAM_G will be overwritten as temporary data buffer for decoding "
        "process. \n\n");
    m_start = EVE_RAM_G;
    m_size  = m_start + size;
    memZero(m_start, m_size);
}

DisplayList * RamG::saveDisplayList(string name) const
{
    if(m_parent->m_cmdBuffer.size() != 0)
    {
        m_parent->m_hal->wr16(REG_CMD_DL, 0);
        m_parent->execute();
    }
    auto * list = new DisplayList(name,
                                  this->m_currentPosition,
                                  m_parent->m_hal->rd16(REG_CMD_DL));
    //Check if DL memory have data to store
    if(list->size() == 0)
    {
        //Try to execute something from CoPro FIFO to DL when user forgot it
        m_parent->execute();
        //Check again
        //        if((list->m_size = m_parent->m_hal->rd16(REG_CMD_DL)) == 0)
        //        {
        debug("Nothing to store. Exit \n");
        delete list;
        return nullptr;
        //        }
    }

    if(list->address() + list->size() > this->m_size)
    {
        error("Display List more than RamG free space!\n");
    }
    memCopy(list->address(), EVE_RAM_DL, list->size());
    m_parent->m_hal->wr16(REG_CMD_DL, 0);
    this->m_currentPosition += list->size();
    m_pool.push_back(list);
    return list;
}

DisplayList * RamG::updateDisplayList(DisplayList * list) const
{
    //Check if DL memory have data to store
    uint16_t dlSize = m_parent->m_hal->rd16(REG_CMD_DL);
    if(dlSize == 0)
    {
        //Try to execute something from CoPro FIFO to DL when user forgot it
        m_parent->execute();
        //Check again
        if((dlSize = m_parent->m_hal->rd16(REG_CMD_DL)) == 0)
        {
            debug("Nothing to update. Exit \n");
            return list;
        }
    }

    if(dlSize > list->size())
    {
        debug("Current DL is more than stored. Store new\n");
        auto newList = saveDisplayList(list->name());
        removeDisplayList(list);
        return newList;
    }
    memCopy(list->address(), EVE_RAM_DL, list->size());
    m_parent->m_hal->wr16(REG_CMD_DL, 0);
    return list;
}

Snapshot * RamG::saveSnapshot(std::string          name,
                              SnapshotBitmapFormat fmt,
                              int16_t              x,
                              int16_t              y,
                              uint16_t             width,
                              uint16_t             height) const
{
    if(m_parent->m_cmdBuffer.size() != 0)
    {
        m_parent->m_hal->wr16(REG_CMD_DL, 0);
        m_parent->execute();
    }
    //Correct image size if it more than screen size
    if(x + width > EVE_HSIZE)
        width = EVE_HSIZE - x;
    if(y + height > EVE_VSIZE)
        height = EVE_VSIZE - y;

    auto * snapshot = new Snapshot(name,
                                   this->m_currentPosition,
                                   x,
                                   y,
                                   width,
                                   height,
                                   fmt);
    if(snapshot->address() + snapshot->size() > this->m_size)
    {
        error("Snapshot more than RamG free space!\n");
    }

    m_parent->push(CMD_SNAPSHOT2);                 //Snapshot command
    m_parent->push(static_cast<uint32_t>(fmt));    //Bitmap Format
    m_parent->push(this->m_currentPosition);       //Pointer to RamG address
    m_parent->push({x, y});                        //Position
    m_parent->push({fmt == SnapshotBitmapFormat::ARGB8_s
                        ? static_cast<int16_t>(width * 2u)
                        : static_cast<int16_t>(width),    //Bitmap Format
                    static_cast<int16_t>(height)});       // Size
    m_parent->execute();                                  //Take a snapsot

    //    debug("Size: %lu, %u\n", EVE_RAM_G_SAFETY_SIZE, snapshot->size());
    //increace current position
    this->m_currentPosition += snapshot->size();
    m_pool.push_back(snapshot);
    //    m_parent->m_hal->wr16(REG_CMD_DL, 0);
    return snapshot;
}

Snapshot * RamG::updateSnapshot(Snapshot * s) const
{
    if(m_parent->m_cmdBuffer.size() != 0)
    {
        m_parent->m_hal->wr16(REG_CMD_DL, 0);
        m_parent->execute();
    }
    m_parent->push(CMD_SNAPSHOT2);                         //Snapshot command
    m_parent->push(static_cast<uint32_t>(s->format()));    //Bitmap Format
    m_parent->push(s->address());                          //Pointer to RamG address
    m_parent->push({s->x(), s->y()});                      //Position
    m_parent->push({s->format() == SnapshotBitmapFormat::ARGB8_s
                        ? static_cast<int16_t>(s->width() * 2u)
                        : static_cast<int16_t>(s->width()),    //Bitmap Format
                    static_cast<int16_t>(s->height())});       // Size
    m_parent->execute();                                       //Take a snapsot
    return s;
}

void RamG::memCopy(uint32_t dest, uint32_t src, uint32_t num) const
{
    if(dest + num > m_size)
    {
        debug("RamG memCopy out of memory\n");
        return;
    }
    m_parent->push(CMD_MEMCPY);
    m_parent->push(dest);
    m_parent->push(src);
    m_parent->push(num);
    m_parent->execute();
}

void RamG::memZero(uint32_t ptr, uint32_t num) const
{
    if(ptr + num > m_size)
    {
        debug("RamG memZero out of memory\n");
        return;
    }
    m_parent->push(CMD_MEMZERO);
    m_parent->push(ptr);
    m_parent->push(num);
    m_parent->execute();
}

void RamG::memSet(uint32_t ptr, uint8_t value, uint32_t num) const
{
    if(ptr + num > m_size)
    {
        debug("RamG memSet out of memory\n");
        return;
    }
    m_parent->push(CMD_MEMSET);
    m_parent->push(ptr);
    m_parent->push(value);
    m_parent->push(num);
    m_parent->execute();
}

void RamG::removeStoredObject(StoredObject * o) const
{
    m_pool.erase(
        std::remove(m_pool.begin(),
                    m_pool.end(),
                    o),
        m_pool.end());
    memZero(o->address(), o->size());
    delete o;
    alignMemory();
}

void RamG::removeStoredObject(string name) const
{
    for(auto it = m_pool.begin(); it != m_pool.end();)    // No ++ here
    {
        if(it.operator*()->name() == name)
        {
            memZero(it.operator*()->address(),
                    it.operator*()->size());
            delete *it;
            it = m_pool.erase(it);
        }
        else
        {
            ++it;
        }
    }
    alignMemory();
}

void RamG::alignMemory() const
{
    if(m_pool.size() == 0)
    {
        m_currentPosition = 0;
        return;
    }
    std::sort(m_pool.begin(), m_pool.end(), StoredObject::compareAddress);
    uint32_t next{0}, addrIt{0};
    for(auto it = m_pool.begin();
        it != m_pool.end();
        ++it)
    {
        if(it.operator*()->address() != addrIt)
            break;
        addrIt += it.operator*()->size();
        ++next;
    }

    if(m_pool.size() == next)
    {
        m_currentPosition = m_pool.back()->address()
                            + m_pool.back()->size();
        return;
    }

    uint32_t nextAddress = m_pool.at(next)->address();
    uint32_t gap         = nextAddress - addrIt;

    for(auto it = m_pool.begin() + next;
        it != m_pool.end();
        ++it)
    {
        it.operator*()->setAddress(it.operator*()->address() - gap);
    }

    while(addrIt < m_currentPosition)
    {
        if(addrIt + gap > m_currentPosition)
        {
            uint32_t lastgap = gap - (m_currentPosition - addrIt);
            memZero(addrIt, lastgap);
            memCopy(addrIt, nextAddress, lastgap);
            break;
        }
        memZero(addrIt, gap);
        memCopy(addrIt, nextAddress, gap);
        addrIt += gap;
        nextAddress += gap;
    }
    m_currentPosition -= gap;
    alignMemory();
}

const std::vector<StoredObject *> & RamG::pool() const
{
    return m_pool;
}
#if defined(BT81X_ENABLE)
Flash::Flash(FT8xx * parent, uint32_t size) :
    m_parent(parent),
    m_size(size)
{
    uint8_t t;
    //    m_flashStatus = static_cast<FlashStatus>(m_parent->m_hal->rd8(REG_FLASH_STATUS));

    while((m_flashStatus = static_cast<FlashStatus>(m_parent->m_hal->rd8(REG_FLASH_STATUS)))
          == FLASH_STATUS_INIT)
    {
        ThisThread::sleep_for(1);
        t++;
        if(t > 100)
        {
            debug("Flash attach error\n");
            delete this;
            return;
        }
    }
    if(m_flashStatus == FLASH_STATUS_DETACHED) /*  - no flash was found during init, no flash present or the detection failed, but have hope and let the BT81x have annother try */
    {
        m_parent->push(CMD_FLASHATTACH);
        m_parent->execute();
        if((m_flashStatus
            = static_cast<FlashStatus>(m_parent->m_hal->rd8(REG_FLASH_STATUS)))
           != FLASH_STATUS_BASIC)
        {
            debug("Flash attach error\n");
            return;
        }
    }
    if(m_flashStatus == FLASH_STATUS_BASIC)
    {
        m_parent->push(CMD_FLASHFAST);
        m_parent->push({0});
        m_parent->execute();
        uint32_t pointer = m_parent->m_hal->rd16(REG_CMD_READ);
        uint32_t ffRes   = m_parent->m_hal->rd32(EVE_RAM_CMD + pointer - 4);
        if(ffRes != FlashInitResult::FlashSucsess)
        {
            debug("Flash error: %#08x\n", ffRes);
            return;
        }
    }

    if((m_flashStatus = static_cast<FlashStatus>(m_parent->m_hal->rd8(REG_FLASH_STATUS))) == FLASH_STATUS_FULL)
    {
        return;
    }
    debug("Flash Init Failed\n");
}

FlashStatus Flash::flashStatus() const
{
    return m_flashStatus;
}
#endif

uint32_t StoredObject::address() const
{
    return m_address;
}

uint32_t StoredObject::size() const
{
    return m_size;
}

void StoredObject::setAddress(const uint32_t & address)
{
    m_address = address;
}

std::string StoredObject::name() const
{
    return m_name;
}

StoredObjectType StoredObject::type() const
{
    return m_type;
}

SnapshotBitmapFormat Snapshot::format() const
{
    return m_format;
}

uint16_t Snapshot::width() const
{
    return m_width;
}

uint16_t Snapshot::height() const
{
    return m_height;
}

int16_t Snapshot::y() const
{
    return m_y;
}

int16_t Snapshot::x() const
{
    return m_x;
}
