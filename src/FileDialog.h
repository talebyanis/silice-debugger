#pragma once

#include <string>
#include <vector>

static const std::vector<const char*> OFD_EXTENSIONS = std::vector<const char*>({ "*.ice", "*.lua" });

static const std::vector<const char*> OFD_FILTER_LUA    = std::vector<const char*>({ "*.lua" });
static const std::vector<const char*> OFD_FILTER_ICE    = std::vector<const char*>({ "*.ice" });
static const std::vector<const char*> OFD_FILTER_ALL    = std::vector<const char*>({ "*.*" });

std::string newFileDialog (                                      const std::vector<const char*>& filter);
std::string openFileDialog(                                      const std::vector<const char*>& filter);
std::string openFileDialog(const char* directory               , const std::vector<const char*>& filter);
std::string saveFileDialog(const char* proposedFileNameFullPath, const std::vector<const char*>& filter);

std::string openFolderDialog(const char* proposedFolderNameFullPath);
