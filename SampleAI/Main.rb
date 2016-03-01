# -*- coding: utf-8 -*-
class Player
  attr_reader :map

  def initialize souls, map, skill_count
    @souls = souls
    @map   = map
    @skill_count = skill_count
  end
end

class Map
  attr_reader :row, :col, :cells, :ninjas, :items

  class Cell
    attr_reader :state, :point
    def initialize x, y, state
      @point = Point.new(x, y)
      @state = state
    end
    def has_object?
      self.state == "W" || self.state == "O"
    end
    def x
      @point.x
    end
    def y
      @point.y
    end
  end

  def initialize row, col, cells, ninjas, enemies, items
    @row = row
    @col = col
    @cells = Array.new(@row){ Array.new(@col) }

    cells.each_with_index do |rows, x|
      rows.each_with_index do |state, y|
        @cells[x][y] = Cell.new(x, y, state)
      end
    end

    @ninjas  = ninjas
    @enemies = enemies
    @items   = items
  end

  def nearest_item x, y
    distances = self.items.map{|i| Math.sqrt((i.x - x).abs**2 + (i.y - y).abs**2) }
    idx = distances.index(distances.min)
    return self.items[idx]
  end

  def movable? point
    !self.cells[point.x][point.y].has_object?
  end

  def shortest_road start_point, end_point
    # 初期化
    map_scores = Array.new(self.row) { Array.new(self.col, nil) }
    map_scores.each_with_index do |row, x|
      row.each_with_index do |cell, y|
        map_scores[x][y] = -1 if self.cells[x][y].has_object?
      end
    end

    i = 0
    map_scores[start_point.x][start_point.y] = 0
    while map_scores.flatten.include?(nil) && i < 100
      map_scores.each_with_index do |row, x|
        row.each_with_index do |score, y|
          if score == i
            point = self.cells[x][y].point
            [point.left, point.right, point.up, point.down].each do |po|
              map_scores[po.x][po.y] = i+1 if self.movable?(po) && map_scores[po.x][po.y] == nil
            end
          end
        end
      end
      i += 1
    end

    step  = map_scores[end_point.x][end_point.y]
    steps = []

    return steps if step.nil? || step < 0
    next_po = self.cells[end_point.x][end_point.y].point
    (step-1).downto(0) do |score|
      [[next_po.right, "R"], [next_po.left, "L"], [next_po.down, "D"], [next_po.up, "U"]].each do |po, code|
        if map_scores[po.x][po.y] == score
          steps.push code
          next_po = po
          break
        end
      end
    end
    return steps.reverse
  end
end


class Point
  attr_reader :x, :y, :left, :right, :up, :down
  def initialize x, y
    @x = x
    @y = y
  end
  def left
    @left ||= Point.new(x, y+1)
  end
  def right
    @right ||= Point.new(x, y-1)
  end
  def up
    @up ||= Point.new(x+1, y)
  end
  def down
    @down ||= Point.new(x-1, y)
  end
end

class Character
  attr_reader :id, :point
  def initialize id, x, y
    @id = id
    @point = Point.new(x, y)
  end
  def x
    @point.x
  end
  def y
    @point.y
  end
end

class Item
  attr_reader :x, :y

  def initialize x, y
    @x = x
    @y = y
  end
end

class Skill
  def initialize id, cost
    @id   = id
    @cost = cost
  end
end

class AI
  def initialize name
    @name = name
  end

  def think
    puts @name
    $stdout.flush

    while true
      # 制限時間
      @timelimit = $stdin.gets.strip

      # スキルの吸い出し
      skill_num = $stdin.gets.strip
      costs     = $stdin.gets.strip.split(" ").map{|cost| cost.to_i}
      @skills   = []
      costs.each_with_index do |cost, id|
        @skills.push Skill.new id, cost
      end

      # 各プレイヤーの情報
      # 順番に注意！（inputは自分、相手の順に情報が入ってくる）
      @me    = extract_player $stdin
      @rival = extract_player $stdin

      # スキルは使わないので2固定
      puts "2"

      # 忍者の行動決定
      @me.map.ninjas.each do |ninja|
        item  = @me.map.nearest_item ninja.x, ninja.y
        steps = @me.map.shortest_road ninja, item
        puts steps[0,2].join("")
      end

      $stdout.flush
    end
  end

  def extract_player file
    # スキルを使えるポイント情報を吸い上げる
    souls = file.gets.strip

    # mapの情報を吸い上げる
    size = file.gets.split(" ")
    row = size[0].to_i
    col = size[1].to_i
    cells = Array.new(row){ Array.new(col) }
    row.times do |x|
      line = file.gets.strip.split("")
      line.each_with_index do |state, y|
        cells[x][y] = state
      end
    end

    # ユニットの情報を吸い上げる
    ninjas  = extract_character file
    enemies = extract_character file
    items   = extract_item file

    # スキル使用回数を吸い上げる
    skill_count = file.gets.split(" ")

    map    = Map.new(row, col, cells, ninjas, enemies, items)
    player = Player.new(souls, map, skill_count)

    return player
  end

  def extract_character file
    tmp = []
    num = file.gets.strip.to_i
    num.times do |i|
      chars = file.gets.strip.split(" ")
      tmp.push Character.new(chars[0].to_i, chars[1].to_i, chars[2].to_i)
    end
    return tmp
  end

  def extract_item file
    tmp = []
    num = file.gets.strip.to_i
    num.times do |i|
      chars = file.gets.strip.split(" ")
      tmp.push Item.new(chars[0].to_i, chars[1].to_i)
    end
    return tmp
  end
end

# AI起動
ai = AI.new "SampleAI.rb"
ai.think

