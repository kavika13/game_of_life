for index, filename in ipairs(tup.glob('*.cpp')) do
  if tup.getconfig('TUP_PLATFORM') != 'macosx' or not string.find(filename, 'ResourcePath.cpp') then
    tup.rule(filename, '!cxx', {'$(INTERMEDIATE_DIR)/%B.o', '../<objs>'})
  end
end

if tup.getconfig('TUP_PLATFORM') == 'macosx' then
  for index, filename in ipairs(tup.glob('*.mm')) do
    tup.rule(filename, '!cxx', {'$(INTERMEDIATE_DIR)/%B.o', '../<objs>'})
  end
end
