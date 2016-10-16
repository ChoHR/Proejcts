import tensorflow as tf

x_data = [[1,1,1,1,1],
          [1,0,3,0,5],
          [0,2,0,4,0]]
y_data = [1,2,3,4,5]

w = tf.Variable(tf.random_uniform([1,3], -1.0, 1.0))

X = tf.placeholder(tf.float32)
Y = tf.placeholder(tf.float32)

hypothesis = tf.matmul(w, X)
                
cost = tf.reduce_mean(tf.square(hypothesis - Y))

a = tf.Variable(0.1) # Learning rate, alpha
optimizer = tf.train.GradientDescentOptimizer(a)
train = optimizer.minimize(cost)

init = tf.initialize_all_variables()

sess = tf.Session()
sess.run(init)

for step in xrange(1001):
    sess.run(train, feed_dict={X:x_data, Y:y_data})
    if step % 20 == 0:
        print step, sess.run(cost, feed_dict={X:x_data, Y:y_data}), sess.run(w)

# Learns best fit is W: [1], b: [0]
#print sess.run(hypothesis, feed_dict={X1:5, X2:4})
#print sess.run(hypothesis, feed_dict={X1:2.5, X2:1.5})