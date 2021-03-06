// Copyright (c) 2018 Johannes Delport
// This code is licensed under MIT license (see LICENSE for details)
#include "j_misc.hpp"

/*
  Returns the file part of a path if a path is present else returns entire path
  (file)
*/
std::string
file_from_path(std::string path)
{
  auto posLastSlash = path.find_last_of("/\\");
  if (posLastSlash == std::string::npos) {
    posLastSlash = 0;
    return path.substr(posLastSlash);
  } else
    return path.substr(posLastSlash + 1);
}
/*
  Returns false if the file is not of specified suffix.
  Dietrich Epp (Dec 7, 2013)
  https://stackoverflow.com/questions/20446201/how-to-check-if-string-ends-with-txt
*/
bool
has_suffix(const std::string& str, const std::string& suffix)
{
  return str.size() >= suffix.size() &&
         str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}
/*
  Returns true or false if a string starts with a character ignoring prepended
  spaces
*/
bool
starts_with(std::string input, char test)
{
  for (auto i : input) {
    if (i != ' ') {
      return i == test;
    }
  }
  return false;
}
/*
  Displays the circuit statistics such as JJ count and component count.
  1 or 0 sets full stats or not. Debug setting only. Not available to user.
*/
void
circuit_stats(int full, InputFile& iFile)
{
  std::stringstream ss;
  std::cout << "Circuit characteristics:" << std::endl;
  std::cout << std::setw(35) << std::left
            << "Subcircuits:" << iFile.subCircuitCount << std::endl;
  std::cout << std::endl;
  if (full == 1) {
    for (auto i : iFile.subcircuitSegments) {
      std::cout << "Subcircuit: " << i.first << std::endl;
      std::cout
        << "*************************************************************"
        << std::endl;
      for (const auto& j : i.second.lines) {
        std::cout << j << std::endl;
      }
      std::cout
        << "*************************************************************"
        << std::endl;
      std::cout << i.first << " component count: "
                << i.second.componentCount << std::endl;
      std::cout << i.first << " JJ count: " << i.second.jjCount
                << std::endl;
      std::cout << std::endl;
    }
    std::cout << "Main circuit: " << std::endl;
    std::cout << "*************************************************************"
              << std::endl;
    for (const auto& i : iFile.maincircuitSegment) {
      std::cout << i << std::endl;
    }
    std::cout << "*************************************************************"
              << std::endl;
    std::cout << "Main circuit component count: " << iFile.componentCount
              << std::endl;
    std::cout << "Main circuit JJ count: " << iFile.jjCount << std::endl;
    std::cout << std::endl;
    std::cout << "Simulation control: " << std::endl;
    std::cout << "*************************************************************"
              << std::endl;
    for (const auto& i : iFile.controlPart) {
      std::cout << i << std::endl;
    }
    std::cout << "*************************************************************"
              << std::endl;
  } else {
    for (auto i : iFile.subcircuitSegments) {
      ss.str(i.first + " component count:");
      std::cout << std::setw(35) << std::left << ss.str()
                << i.second.componentCount << std::endl;
      ss.str(i.first + " JJ count:");
      std::cout << std::setw(35) << std::left << ss.str()
                << i.second.jjCount << std::endl;
    }
    std::cout << std::endl;
    std::cout << std::setw(35) << std::left
              << "Main circuit component count:" << iFile.circuitComponentCount
              << std::endl;
    std::cout << std::setw(35) << std::left
              << "Main circuit JJ count:" << iFile.circuitJJCount << std::endl;
    std::cout << std::endl;
  }
}
/*
  Turn string into tokens using only spaces
*/
std::vector<std::string>
tokenize_space(std::string c)
{
  std::string::size_type pos, lastPos = 0, length = c.length();
  std::string delimiters = " \t";
  std::vector<std::string> tokens;
  bool trimEmpty = true;

   using value_type = typename std::vector<std::string>::value_type;
   using size_type  = typename std::vector<std::string>::size_type;

   while(lastPos < length + 1)
   {
      pos = c.find_first_of(delimiters, lastPos);
      if(pos == std::string::npos)
      {
         pos = length;
      }

      if(pos != lastPos || !trimEmpty)
         tokens.push_back(value_type(c.data()+lastPos,
               (size_type)pos-lastPos ));

      lastPos = pos + 1;
   }
   return tokens;
}
/*
  Turn string into tokens using any delimiter (slower)
*/
std::vector<std::string>
tokenize_delimeter(std::string c, std::string d)
{
  std::vector<std::string> tokens;
  std::stringstream stringStream(c);
  std::string line;
  while (std::getline(stringStream, line)) {
    std::size_t prev = 0, pos;
    while ((pos = line.find_first_of(d, prev)) != std::string::npos) {
      if (pos > prev)
        tokens.push_back(line.substr(prev, pos - prev));
      prev = pos + 1;
    }
    if (prev < line.length())
      tokens.push_back(line.substr(prev, std::string::npos));
  }
  return tokens;
}
/*
  Count values equal to specified value in given map
*/
int
map_value_count(std::unordered_map<std::string, int> map, int value)
{
  int counter = 0;
  for (auto i : map) {
    if (i.second == value)
      counter++;
  }
  return counter;
}
/*
  Takes a string and returns double version with correct modifier
  Eg. "5PS" returns 5E-12
*/
double
modifier(std::string value)
{
  std::string::size_type sz;
  double number;
  try {
    number = std::stod(value, &sz);
  } catch (const std::invalid_argument&) {
    misc_errors(STOD_ERROR, value);
  }
  switch (value.substr(sz)[0]) {
      /* mega */
    case 'X':
      return number * 1E6;
      /* mega or milli */
    case 'M':
      /* mega */
      if (value.substr(sz)[1] == 'E' && value.substr(sz)[2] == 'G')
        return number * 1E6;
      /* milli */
      else
        return number * 1E-3;
      /* micro */
    case 'U':
      return number * 1E-6;
      /* nano */
    case 'N':
      return number * 1E-9;
      /* pico */
    case 'P':
      return number * 1E-12;
      /* femto */
    case 'F':
      return number * 1E-15;
      /* kilo */
    case 'K':
      return number * 1E3;
      /* giga */
    case 'G':
      return number * 1E9;
      /* tera */
    case 'T':
      return number * 1E12;
      /* auto modifier */
    case 'E':
      return std::stod(value);
    default:
      return number;
  }
}
/*
  Unique vector push. Only pushes if item being pushed is unique
*/
void
unique_push(std::vector<std::string>& vector, std::string string)
{
  if (std::find(vector.begin(), vector.end(), string) == vector.end()) {
    vector.push_back(string);
  }
}
/*
  Fetch index of value in vector, if not found return -1
*/
int
index_of(std::vector<std::string> vector, std::string value)
{
  int counter = 0;
  for (const auto& i : vector) {
    /* Value found, return counter */
    if (value == vector[counter])
      return counter;
    counter++;
  }
  /* Value was not found, set counter equal to -1 and return */
  counter = -1;
  return counter;
}
/*
  Substring from after specified string
*/
std::string
substring_after(std::string str, std::string whatpart)
{
  std::size_t pos = 0;
  std::string substring;
  if (str.find(whatpart) != std::string::npos)
    pos = str.find(whatpart) + whatpart.size();
  substring = str.substr(pos);
  return substring;
}
/*
Substring from before specified string
*/
std::string
substring_before(std::string str, std::string whatpart)
{
  std::size_t pos = 0;
  std::string substring;
  if (str.find(whatpart) != std::string::npos) {
    pos = str.find(whatpart);
    substring = str.substr(0, pos);
    return substring;
  } else
    return str;
}
/*
  Function parser. Turns a string of pwl, pulse, sin, cos into a vector of
  values for the duration of the simulation.
*/
std::vector<double>
function_parse(std::string str, InputFile& iFile)
{
  std::vector<double> functionOfT(tsim.simsize(), 0.0);
  std::vector<std::string> tokens;
  std::string posVarName, subckt = "";
  tokens = tokenize_space(str);
  if(tokens[0].find_first_of("|") != std::string::npos) {
    subckt = iFile.subcircuitNameMap[tokens[0].substr(tokens[0].find_first_of("|") + 1)];
  }
  /* Identify string parrameter part of the string */
  auto first = str.find('(') + 1;
  auto last = str.find(')');
  std::string params = str.substr(first, last - first);
  tokens = tokenize_delimeter(params, " ,");
  /* Determine type of source and handle accordingly*/
  /* PWL */
  if (str.find("PWL") != std::string::npos) {
    if (std::stod(tokens[0]) != 0.0 || std::stod(tokens[1]) != 0.0)
      function_errors(INITIAL_VALUES, tokens[0] + " & " + tokens[1]);
    std::vector<double> timesteps, values;
    for (int i = 0; i < tokens.size(); i = i + 2) {
      if (modifier(tokens[i]) > tsim.tstop) {
        timesteps.push_back(modifier(tokens[i - 2]));
      } else
        timesteps.push_back(modifier(tokens[i]));
    }
    for (int i = 1; i < tokens.size(); i = i + 2) {
      if (iFile.parVal.find(tokens[i]) != iFile.parVal.end())
        values.push_back(iFile.parVal[tokens[i]]);
      else if (iFile.subcircuitSegments[subckt].parVal.find(tokens[i]) != iFile.subcircuitSegments[subckt].parVal.end())
        values.push_back(iFile.subcircuitSegments[subckt].parVal[tokens[i]]);
      else
        values.push_back(modifier(tokens[i]));
    }
    if (timesteps.size() < values.size())
      function_errors(TOO_FEW_TIMESTEPS,
                      std::to_string(timesteps.size()) + " timesteps & " +
                        std::to_string(timesteps.size()) + " values");
    if (timesteps.size() > values.size())
      function_errors(TOO_FEW_VALUES,
                      std::to_string(timesteps.size()) + " timesteps & " +
                        std::to_string(timesteps.size()) + " values");
    if (values[values.size() - 1] != 0.0) {
      std::fill(functionOfT.begin() +
                  timesteps[timesteps.size() - 1] / tsim.maxtstep,
                functionOfT.end(),
                values[values.size() - 1]);
    }
    double startpoint, endpoint, value = 0.0;
    for (int i = 1; i < timesteps.size(); i++) {
      startpoint = timesteps[i - 1] / tsim.maxtstep;
      endpoint = timesteps[i] / tsim.maxtstep;
      for (int j = startpoint; j < endpoint; j++) {
        if (values[i - 1] < values[i])
          value = values[i] / (endpoint - startpoint) * (j - (int)startpoint);
        else if (values[i - 1] > values[i])
          value = values[i - 1] - (values[i - 1] / (endpoint - startpoint) *
                                   (j - (int)startpoint));
        else if (values[i - 1] == values[i])
          value = values[i];
        functionOfT[j] = value;
      }
    }
  }
  /* PULSE */
  else if (str.find("PULSE") != std::string::npos) {
    if (std::stod(tokens[0]) != 0.0)
      function_errors(INITIAL_PULSE_VALUE, tokens[0]);
    if (tokens.size() < 7)
      function_errors(PULSE_TOO_FEW_ARGUMENTS, std::to_string(tokens.size()));
    double vPeak, timeDelay, timeRise, timeFall, pulseWidth, pulseRepeat;
    vPeak = modifier(tokens[1]);
    if (vPeak == 0.0)
      if (VERBOSE)
        function_errors(PULSE_VPEAK_ZERO, tokens[1]);
    timeDelay = modifier(tokens[2]);
    timeRise = modifier(tokens[3]);
    timeFall = modifier(tokens[4]);
    pulseWidth = modifier(tokens[5]);
    if (pulseWidth == 0.0)
      if (VERBOSE)
        function_errors(PULSE_WIDTH_ZERO, tokens[5]);
    pulseRepeat = modifier(tokens[6]);
    if (pulseRepeat == 0.0)
      if (VERBOSE)
        function_errors(PULSE_REPEAT, tokens[6]);
    int PR = pulseRepeat / tsim.maxtstep;
    int TD = timeDelay / tsim.maxtstep;
    std::vector<double> timesteps, values;
    double timestep;
    for (int i = 0; i < ((tsim.simsize() - TD) / PR); i++) {
      timestep = timeDelay + (pulseRepeat * i);
      if (timestep < tsim.tstop)
        timesteps.push_back(timestep);
      else
        break;
      values.push_back(0.0);
      timestep = timeDelay + (pulseRepeat * i) + timeRise;
      if (timestep < tsim.tstop)
        timesteps.push_back(timestep);
      else
        break;
      values.push_back(vPeak);
      timestep = timeDelay + (pulseRepeat * i) + timeRise + pulseWidth;
      if (timestep < tsim.tstop)
        timesteps.push_back(timestep);
      else
        break;
      values.push_back(vPeak);
      timestep =
        timeDelay + (pulseRepeat * i) + timeRise + pulseWidth + timeFall;
      if (timestep < tsim.tstop)
        timesteps.push_back(timestep);
      else
        break;
      values.push_back(0.0);
    }
    double startpoint, endpoint, value;
    for (int i = 1; i < timesteps.size(); i++) {
      startpoint = timesteps[i - 1] / tsim.maxtstep;
      endpoint = timesteps[i] / tsim.maxtstep;
      for (int j = startpoint; j < endpoint; j++) {
        if (values[i - 1] < values[i])
          value = values[i] / (endpoint - startpoint) * (j - (int)startpoint);
        else if (values[i - 1] > values[i])
          value = values[i - 1] - (values[i - 1] / (endpoint - startpoint) *
                                   (j - (int)startpoint));
        else
          value = values[i];
        functionOfT[j] = value;
      }
    }
  }
  return functionOfT;
}
/*
        Helper function for finding the depth of subcircuits in the design
*/
bool
findX(std::vector<std::string>& segment, std::string& theLine)
{
  for (auto i : segment) {
    if (i[0] == 'X') {
      theLine = i;
      return true;
    }
  }
  return false;
}
/*
        Function that finds the depth of the subcircuits in the design
*/
int
subCircuitDepth(std::vector<std::string> segment,
                InputFile& iFile,
                int& thisDepth,
                int& overallDepth)
{
  std::string subcktLine, subcktName;
  std::vector<std::string> tokens;
  if (findX(segment, subcktLine)) {
    tokens = tokenize_space(subcktLine);
    thisDepth++;
    if (thisDepth > overallDepth)
      overallDepth = thisDepth;
    // Check if the second token can be identified as a subcircuit name. If yes
    // then
    if (iFile.subcircuitSegments.find(tokens[1]) !=
        iFile.subcircuitSegments.end()) {
      // Identify the type of subcircuit
      subcktName = tokens[1];
    } else if (iFile.subcircuitSegments.find(tokens.back()) !=
               iFile.subcircuitSegments.end()) {
      // Identify the type of subcircuit
      subcktName = tokens.back();
    } else {
      // The subcircuit name was not found therefore error out
      invalid_component_errors(MISSING_SUBCIRCUIT_NAME, subcktLine);
    }
    subCircuitDepth(iFile.subcircuitSegments[subcktName].lines,
                    iFile,
                    thisDepth,
                    overallDepth);
  } else
    thisDepth = 1;
  return overallDepth;
}
/*
  Compare two strings. Return difference
*/
std::string stringSubtract(std::string src, std::string comp)
{
  int srcIt = src.size() - 1, compIt = comp.size() - 1;
  std::string rslt = src;
  return rslt;
}
