local function trim(value)
  return value:gsub("\194\160", ""):match("^%s*(.-)%s*$")
end

local function split_cells(line)
  local cells = {}
  local start = 1
  while #cells < 9 do
    local separator = line:find("&", start, true)
    if not separator then
      return nil
    end
    table.insert(cells, trim(line:sub(start, separator - 1)))
    start = separator + 1
  end
  table.insert(cells, trim(line:sub(start)))
  return cells
end

local function extract_lines(el)
  local lines = {}
  for _, block in ipairs(el.content) do
    local current = pandoc.Inlines({})
    for _, inline in ipairs(block.content or {}) do
      if inline.t == "LineBreak" then
        table.insert(lines, trim(pandoc.utils.stringify(current)))
        current = pandoc.Inlines({})
      elseif inline.t ~= "Span" then
        current:insert(inline)
      end
    end
    table.insert(lines, trim(pandoc.utils.stringify(current)))
  end
  return lines
end

local function is_description(value)
  return value:match("^Air mass 1%.5")
    or value:match("^37 degree tilted")
    or value:match("^Based on ISO")
    or value:match("^ISO%-9845")
    or value:match("^Photopic response")
    or value:match("^Based on CIE")
    or value:match("^derived from Optics5")
    or value:match("^which is the same")
    or value:match("^Wavelengths %(microns%)")
end

local function text_blocks(value)
  return { pandoc.Plain({ pandoc.Str(value) }) }
end

local function cell_text(cell)
  return trim(pandoc.utils.stringify(cell.contents or {}))
end

local function row_is_empty(row)
  for _, cell in ipairs(row.cells or {}) do
    if cell_text(cell) ~= "" then
      return false
    end
  end
  return true
end

local function rows_match(first, second)
  local first_cells = first.cells or {}
  local second_cells = second.cells or {}
  if #first_cells ~= #second_cells then
    return false
  end

  for index, cell in ipairs(first_cells) do
    if cell_text(cell) ~= cell_text(second_cells[index]) then
      return false
    end
  end
  return true
end

local function remove_repeated_header(el)
  local head_rows = el.head and el.head.rows or {}
  if #head_rows == 0 then
    return
  end

  for _, body in ipairs(el.bodies or {}) do
    local body_rows = body.body or {}
    local repeated_header_start = 1

    while body_rows[repeated_header_start]
        and row_is_empty(body_rows[repeated_header_start]) do
      repeated_header_start = repeated_header_start + 1
    end

    local header_matches = true
    for index, head_row in ipairs(head_rows) do
      local body_row = body_rows[repeated_header_start + index - 1]
      if not body_row or not rows_match(head_row, body_row) then
        header_matches = false
        break
      end
    end

    if header_matches then
      local rows_to_remove = repeated_header_start + #head_rows - 1
      for _ = 1, rows_to_remove do
        table.remove(body_rows, 1)
      end
    end
  end
end

function Div(el)
  if not el.classes:includes("tabular") then
    return nil
  end

  local lines = extract_lines(el)
  local rows = {}
  local identifier
  local caption

  for _, line in ipairs(lines) do
    local values = split_cells(line)
    if values then
      if values[1]:match("^Air mass 1%.5") then
        identifier = "table:solar-spectral-irradiance-function"
        caption = "Solar spectral irradiance function."
      elseif values[1]:match("^Photopic response") then
        identifier = "table:photopic-response-function"
        caption = "Photopic response function."
      end

      local cells = {}
      if is_description(values[1]) then
        cells[1] = pandoc.Cell(text_blocks(values[1]), pandoc.AlignLeft, 1, 10)
      else
        for column, value in ipairs(values) do
          cells[column] = pandoc.Cell(text_blocks(value), pandoc.AlignRight)
        end
      end
      table.insert(rows, pandoc.Row(cells))
    end
  end

  if not identifier then
    return nil
  end

  local colspecs = {}
  for _ = 1, 10 do
    table.insert(colspecs, { pandoc.AlignRight })
  end

  return pandoc.Table(
    { long = text_blocks(caption) },
    colspecs,
    pandoc.TableHead({}),
    {
      {
        attr = pandoc.Attr(),
        head = {},
        body = rows,
        row_head_columns = 0,
      },
    },
    pandoc.TableFoot({}),
    pandoc.Attr(identifier, { "table", "table-bordered", "table-striped", "table-sm" })
  )
end

function Span(el)
  if el.identifier == "table:solar-spectral-irradiance-function"
      or el.identifier == "table:photopic-response-function" then
    return {}
  end
end

function Table(el)
  remove_repeated_header(el)
  el.attr.classes:insert("table")
  el.attr.classes:insert("table-bordered")
  el.attr.classes:insert("table-striped")
  el.attr.classes:insert("table-sm")
  return el
end
